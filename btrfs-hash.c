#include "crypto/xxhash.h"
#include "kerncompat.h"
#include "kernel-shared/accessors.h"
#include "kernel-shared/ctree.h"
#include "kernel-shared/disk-io.h"
#include "kernel-shared/extent_io.h"
#include "kernel-shared/uapi/btrfs_tree.h"
#include "kernel-shared/volumes.h"
#include <blkid/blkid.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int btrfs_lookup_csums(
	struct btrfs_trans_handle* trans,
	struct btrfs_root* root,
	struct btrfs_path* path,
	u64 bytenr,
	int total_csums,
	XXH64_hash_t* xxhash
) {
	u16 csum_size = root->fs_info->csum_size;
	int pending_csums = total_csums;
	u8 csum_buf[64]; // Btrfs supports up to 64 bytes (BLAKE2b)

	struct btrfs_key file_key;
	file_key.objectid = BTRFS_EXTENT_CSUM_OBJECTID;
	file_key.offset = bytenr;
	file_key.type = BTRFS_EXTENT_CSUM_KEY;

	int ret = btrfs_search_slot(trans, root, &file_key, path, 0, 0);
	if (ret < 0) goto fail;
	if (ret > 0) {
		if (path->slots[0] == 0) {
			ret = btrfs_prev_leaf(root, path);
			if (ret < 0) goto fail;
			if (ret > 0) {
				ret = -ENOENT;
				goto fail;
			}
		} else {
			path->slots[0]--;
		}
	}

	while (pending_csums > 0) {
		struct extent_buffer* leaf = path->nodes[0];
		int slot = path->slots[0];
		if (slot >= btrfs_header_nritems(leaf)) {
			ret = btrfs_next_leaf(root, path);
			if (ret < 0) goto fail;
			if (ret > 0) break;
			continue;
		}

		struct btrfs_key found_key;
		btrfs_item_key_to_cpu(leaf, &found_key, slot);
		if (found_key.objectid != BTRFS_EXTENT_CSUM_OBJECTID ||
		    found_key.type != BTRFS_EXTENT_CSUM_KEY) {
			break;
		}

		u64 item_start = found_key.offset;
		u32 item_size = btrfs_item_size(leaf, slot);
		u64 item_end = item_start + (item_size / csum_size) * root->fs_info->sectorsize;

		if (bytenr >= item_end || bytenr < item_start) {
			if (bytenr < item_start) break;
			path->slots[0]++;
			continue;
		}

		u32 offset_in_item = (bytenr - item_start) / root->fs_info->sectorsize;
		u32 csums_available = (item_size / csum_size) - offset_in_item;
		u32 csums_to_read = (csums_available > pending_csums) ? pending_csums : csums_available;

		struct btrfs_csum_item* item = btrfs_item_ptr(leaf, slot, struct btrfs_csum_item);
		unsigned long ptr = (unsigned long)item + (offset_in_item * csum_size);
		u32 bytes_to_read = csums_to_read * csum_size;

		void* bulk_csums = malloc(bytes_to_read);
		if (bulk_csums) {
			read_extent_buffer(leaf, bulk_csums, ptr, bytes_to_read);
			*xxhash = XXH64(bulk_csums, bytes_to_read, *xxhash);
			free(bulk_csums);
		} else {
			for (u32 i = 0; i < csums_to_read; i++) {
				read_extent_buffer(leaf, csum_buf, ptr, csum_size);
				*xxhash = XXH64(csum_buf, csum_size, *xxhash);
				ptr += csum_size;
			}
		}

		pending_csums -= csums_to_read;
		bytenr += (u64)csums_to_read * root->fs_info->sectorsize;
		path->slots[0]++;
	}

	return (pending_csums == 0) ? 0 : -ENOENT;

fail:
	return (ret < 0) ? ret : -1;
}

static int btrfs_lookup_extent(
	struct btrfs_fs_info* device_info,
	u64 ino,
	u64 subvolid,
	XXH64_hash_t* xxhash
) {
	int total_csums = 0;
	u32 sectorsize = device_info->sectorsize;

	struct btrfs_root* fs_root;
	if (subvolid == BTRFS_FS_TREE_OBJECTID) {
		fs_root = device_info->fs_root;
	} else {
		struct btrfs_key root_key;
		root_key.objectid = subvolid;
		root_key.type = BTRFS_ROOT_ITEM_KEY;
		root_key.offset = (u64)-1;
		fs_root = btrfs_read_fs_root(device_info, &root_key);
		if (IS_ERR(fs_root)) {
			fprintf(stderr, "Error: Unable to read subvolume %llu\n", (unsigned long long)subvolid);
			return PTR_ERR(fs_root);
		}
	}
	struct btrfs_key key;
	key.objectid = ino;
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = 0;
	struct btrfs_key found_key;
	struct extent_buffer* leaf;
	int slot;
	struct btrfs_path* path1 = btrfs_alloc_path();
	struct btrfs_path *path = btrfs_alloc_path();
	int ret = btrfs_search_slot(NULL, fs_root, &key, path, 0, 0);

	if (ret < 0) {
		fprintf(stderr,
			"Error: btrfs_search_slot failed with %d\n", ret);
		goto error;
	}

	if (ret > 0) {
		leaf = path->nodes[0];
		slot = path->slots[0];
		if (slot >= btrfs_header_nritems(leaf)) {
			ret = btrfs_next_leaf(fs_root, path);
			if (ret < 0) goto error;
			if (ret > 0) {
				ret = -ENOENT;
				goto error;
			}
			leaf = path->nodes[0];
			slot = path->slots[0];
		}
		btrfs_item_key_to_cpu(leaf, &found_key, slot);

		if (found_key.objectid != ino || found_key.type != BTRFS_EXTENT_DATA_KEY) {
			ret = -ENOENT;
			goto error;
		}
	}

	struct btrfs_root* csum_root = btrfs_csum_root(device_info, 0);

	while (1) {
		leaf = path->nodes[0];
		slot = path->slots[0];
		if (slot >= btrfs_header_nritems(leaf)) {
			ret = btrfs_next_leaf(fs_root, path);
			if (ret == 0) continue;
			if (ret < 0) goto error;
			break;
		}

		btrfs_item_key_to_cpu(leaf, &found_key, slot);
		if (found_key.objectid != ino || found_key.type != BTRFS_EXTENT_DATA_KEY) {
			break;
		}

		struct btrfs_file_extent_item* fi = btrfs_item_ptr(leaf, slot,
					struct btrfs_file_extent_item);
		if (btrfs_file_extent_type(leaf, fi) ==
			BTRFS_FILE_EXTENT_INLINE) {
			ret = -1; // Fallback to direct read
			goto error;
		}

		u64 disk_bytenr = btrfs_file_extent_disk_bytenr(leaf, fi);
		if (disk_bytenr == 0) { // Sparse hole
			path->slots[0]++;
			continue;
		}
		u64 disk_num_bytes = btrfs_file_extent_disk_num_bytes(leaf, fi);
		u64 offset = btrfs_file_extent_offset(leaf, fi);
		u64 num_bytes = btrfs_file_extent_num_bytes(leaf, fi);
		u8 compression = btrfs_file_extent_compression(leaf, fi);
		u64 bytenr;

		if (compression) {
			// Compressed: csums cover the compressed on-disk data
			bytenr = disk_bytenr;
			total_csums = disk_num_bytes / sectorsize;
		} else {
			// Uncompressed: csums cover the referenced portion
			bytenr = disk_bytenr + offset;
			total_csums = num_bytes / sectorsize;
		}

		csum_root = btrfs_csum_root(device_info, bytenr);
		ret = btrfs_lookup_csums(
			NULL,
			csum_root,
			path1,
			bytenr,
			total_csums,
			xxhash
		);
		btrfs_release_path(path1);
		if (ret) goto error;
		path->slots[0]++;
	}
	ret = 0;

error:
	btrfs_free_path(path1);
	btrfs_release_path(path);
	btrfs_free_path(path);
	return ret;
}

static char* resolve_device(const char* file_path, u64* subvolid) {
	*subvolid = BTRFS_FS_TREE_OBJECTID;
	char resolved_file_path[PATH_MAX];
	if (!realpath(file_path, resolved_file_path)) {
		fprintf(stderr, "Error: realpath failed for %s: %s\n", file_path, strerror(errno));
		return NULL;
	}

	FILE* mntinfo = fopen("/proc/self/mountinfo", "r");
	if (!mntinfo) {
		fprintf(stderr, "Error: Unable to open /proc/self/mountinfo\n");
		return NULL;
	}

	// find longest mount_path that is prefix of file_path
	size_t mount_path_length_max = 0;
	static char device_path[4096];
	device_path[0] = '\0';
	// read chunk-wise
	char chunk[4096];
	while (fgets(chunk, 4096, mntinfo)) {
		// index of " - ", right before the fs type
		char* result_address = strstr(chunk, " - btrfs ");
		if (!result_address) continue;
		result_address += 3;
		// check if next comes "btrfs "
		if (*result_address != 'b') continue;
		if (*(++result_address) != 't') continue;
		if (*(++result_address) != 'r') continue;
		if (*(++result_address) != 'f') continue;
		if (*(++result_address) != 's') continue;
		if (*(++result_address) != ' ') continue;

		// skip first 4 columns
		char* mount_path = strtok(chunk, " ");
		int column_count = 0;
		while (mount_path) {
			if (++column_count == 5) goto column_found;
			mount_path = strtok(NULL, " ");
		}
		continue;
		column_found:;

		// check if file_path begins with mount_path
		size_t mount_path_length = strlen(mount_path);
		if (mount_path_length > 1) {
			mount_path[mount_path_length] = '/';
			mount_path[++mount_path_length] = '\0';
		}
		if (
			mount_path_length < mount_path_length_max ||
			strncmp(resolved_file_path, mount_path, mount_path_length)
		) continue;

		char* result_end_address = strstr(++result_address, " ");
		if (!result_end_address) continue;
		*result_end_address = '\0';
		mount_path_length_max = mount_path_length;
		strncpy(device_path, result_address, 4095);

		// parse subvolid from mount options after device path
		char* svid = strstr(++result_end_address, "subvolid=");
		if (svid) {
			*subvolid = (u64)strtoull(svid + 9, NULL, 10);
		}
	}
	if (mount_path_length_max) return device_path;

	fprintf(stderr, "Error: Unable to determine block device for %s\n", resolved_file_path);
	return NULL;
}

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: btrfs-hash somehugefile.bin\n");
		return -1;
	}

	char* file_path = argv[1];
	struct stat file_stat;
	if (stat(file_path, &file_stat) < 0) {
		fprintf(stderr, "Error: Unable to access file %s: %s\n", file_path, strerror(errno));
		return -1;
	}
	if (!S_ISREG(file_stat.st_mode)) {
		fprintf(stderr, "Error: %s is not a regular file.\n", file_path);
		return -1;
	}

	u64 subvolid;
	char* device_path = resolve_device(file_path, &subvolid);
	if (!device_path) return -1;
	struct open_ctree_args open_ctree_fs_info_args = {
		.flags = OPEN_CTREE_PARTIAL,
		.filename = device_path,
	};
	struct btrfs_fs_info* device_info = open_ctree_fs_info(&open_ctree_fs_info_args);
	if (!device_info) {
		fprintf(stderr, "Error: Unable to open block device %s\n", device_path);
		return -1;
	}

	XXH64_hash_t xxhash = 0;
	if (
		btrfs_lookup_extent(
			device_info,
			file_stat.st_ino,
			subvolid,
			&xxhash
		)
	) { // if lookup fails, hash the file directly
		fprintf(stderr, "Unable to lookup extent, falling back to direct read...\n");
		int file = open(file_path, O_RDONLY);
		if (file < 0) {
			fprintf(stderr, "Error: Unable to open file %s: %s\n", file_path, strerror(errno));
			return -1;
		}

		unsigned char* buffer = malloc(65536);
		if (!buffer) {
			fprintf(stderr, "Error: Unable to allocate buffer\n");
			return -1;
		}
		ssize_t bytes_read;
		while ((bytes_read = read(file, buffer, 65536)) > 0) {
			xxhash = XXH64(buffer, bytes_read, xxhash);
		}
		close(file);
		free(buffer);
		if (bytes_read < 0) {
			fprintf(stderr, "Error: Unable to read file %s: %s\n", file_path, strerror(errno));
			return -1;
		}
	}

	printf("%016llx\n", (unsigned long long)xxhash);
}
