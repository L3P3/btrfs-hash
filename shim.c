// Thin C shim for btrfs-progs struct field accesses that Zig cannot translate
// (anonymous unions/structs in btrfs_fs_info, btrfs_path, etc. make them opaque)
#include "kerncompat.h"
#include "kernel-shared/accessors.h"
#include "kernel-shared/ctree.h"
#include "kernel-shared/disk-io.h"
#include "kernel-shared/extent_io.h"
#include "kernel-shared/uapi/btrfs_tree.h"
#include "crypto/xxhash.h"

// btrfs_fs_info field accessors
u32 shim_fs_info_sectorsize(const struct btrfs_fs_info *info) {
	return info->sectorsize;
}

u16 shim_fs_info_csum_size(const struct btrfs_fs_info *info) {
	return info->csum_size;
}

struct btrfs_root *shim_fs_info_fs_root(const struct btrfs_fs_info *info) {
	return info->fs_root;
}

// btrfs_root field accessors
struct btrfs_fs_info *shim_root_fs_info(const struct btrfs_root *root) {
	return root->fs_info;
}

// btrfs_path field accessors
struct extent_buffer *shim_path_node(const struct btrfs_path *path, int level) {
	return path->nodes[level];
}

int shim_path_slot(const struct btrfs_path *path, int level) {
	return path->slots[level];
}

void shim_path_set_slot(struct btrfs_path *path, int level, int val) {
	path->slots[level] = val;
}

void shim_path_inc_slot(struct btrfs_path *path, int level) {
	path->slots[level]++;
}

void shim_path_dec_slot(struct btrfs_path *path, int level) {
	path->slots[level]--;
}

// btrfs_item_ptr_offset wrapper
unsigned long shim_item_ptr_offset(struct extent_buffer *leaf, int slot) {
	return btrfs_item_ptr_offset(leaf, slot);
}

// btrfs_file_extent_item accessors (fi is really a byte offset, not a real pointer)
u8 shim_file_extent_type(struct extent_buffer *eb, unsigned long fi_offset) {
	struct btrfs_file_extent_item *fi = (struct btrfs_file_extent_item *)fi_offset;
	return btrfs_file_extent_type(eb, fi);
}

u64 shim_file_extent_disk_bytenr(struct extent_buffer *eb, unsigned long fi_offset) {
	struct btrfs_file_extent_item *fi = (struct btrfs_file_extent_item *)fi_offset;
	return btrfs_file_extent_disk_bytenr(eb, fi);
}

u64 shim_file_extent_disk_num_bytes(struct extent_buffer *eb, unsigned long fi_offset) {
	struct btrfs_file_extent_item *fi = (struct btrfs_file_extent_item *)fi_offset;
	return btrfs_file_extent_disk_num_bytes(eb, fi);
}

u64 shim_file_extent_offset(struct extent_buffer *eb, unsigned long fi_offset) {
	struct btrfs_file_extent_item *fi = (struct btrfs_file_extent_item *)fi_offset;
	return btrfs_file_extent_offset(eb, fi);
}

u64 shim_file_extent_num_bytes(struct extent_buffer *eb, unsigned long fi_offset) {
	struct btrfs_file_extent_item *fi = (struct btrfs_file_extent_item *)fi_offset;
	return btrfs_file_extent_num_bytes(eb, fi);
}

u8 shim_file_extent_compression(struct extent_buffer *eb, unsigned long fi_offset) {
	struct btrfs_file_extent_item *fi = (struct btrfs_file_extent_item *)fi_offset;
	return btrfs_file_extent_compression(eb, fi);
}

// Wrappers for static inline / SETGET_FUNCS generated functions
u32 shim_header_nritems(const struct extent_buffer *eb) {
	return btrfs_header_nritems(eb);
}

void shim_item_key_to_cpu(const struct extent_buffer *eb, struct btrfs_key *key, int nr) {
	btrfs_item_key_to_cpu(eb, key, nr);
}

int shim_next_leaf(struct btrfs_root *root, struct btrfs_path *path) {
	return btrfs_next_leaf(root, path);
}

u32 shim_item_size(const struct extent_buffer *eb, int slot) {
	return btrfs_item_size(eb, slot);
}
