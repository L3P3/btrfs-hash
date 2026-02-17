const std = @import("std");

const stderr = std.io.getStdErr().writer();

// Btrfs types are opaque to Zig (anonymous unions/structs in headers).
// Field access and inline functions are provided by shim.c.
const BtrfsFsInfo = opaque {};
const BtrfsRoot = opaque {};
const BtrfsPath = opaque {};
const ExtentBuffer = opaque {};

const BtrfsKey = extern struct {
	objectid: u64,
	type: u8,
	offset: u64 align(1),
};

const OpenCtreeArgs = extern struct {
	filename: ?[*:0]const u8 = null,
	sb_bytenr: u64 = 0,
	root_tree_bytenr: u64 = 0,
	chunk_tree_bytenr: u64 = 0,
	flags: c_uint = 0,
};

// btrfs_tree.h constants
const BTRFS_EXTENT_CSUM_OBJECTID: u64 = @bitCast(@as(i64, -10));
const BTRFS_FS_TREE_OBJECTID: u64 = 5;
const BTRFS_EXTENT_CSUM_KEY: u8 = 128;
const BTRFS_EXTENT_DATA_KEY: u8 = 108;
const BTRFS_ROOT_ITEM_KEY: u8 = 132;
const BTRFS_FILE_EXTENT_INLINE: u8 = 0;
const OPEN_CTREE_PARTIAL: c_uint = 1 << 1;

// btrfs-progs C functions
extern fn open_ctree_fs_info(oca: *OpenCtreeArgs) ?*BtrfsFsInfo;
extern fn btrfs_read_fs_root(info: *BtrfsFsInfo, key: *BtrfsKey) ?*BtrfsRoot;
extern fn btrfs_csum_root(info: *BtrfsFsInfo, bytenr: u64) ?*BtrfsRoot;
extern fn btrfs_alloc_path() ?*BtrfsPath;
extern fn btrfs_free_path(path: *BtrfsPath) void;
extern fn btrfs_release_path(path: *BtrfsPath) void;
extern fn btrfs_search_slot(
	trans: ?*anyopaque, root: *BtrfsRoot,
	key: *BtrfsKey, path: *BtrfsPath,
	ins_len: c_int, cow: c_int,
) c_int;
extern fn btrfs_prev_leaf(root: *BtrfsRoot, path: *BtrfsPath) c_int;
extern fn read_extent_buffer(
	eb: *const ExtentBuffer, dst: ?*anyopaque,
	start: c_ulong, len: c_ulong,
) void;
extern fn XXH64(input: ?*const anyopaque, length: usize, seed: u64) u64;

// shim.c wrappers for inline/macro functions and opaque struct fields
extern fn shim_fs_info_sectorsize(info: *const BtrfsFsInfo) u32;
extern fn shim_fs_info_csum_size(info: *const BtrfsFsInfo) u16;
extern fn shim_fs_info_fs_root(info: *const BtrfsFsInfo) ?*BtrfsRoot;
extern fn shim_root_fs_info(root: *const BtrfsRoot) *BtrfsFsInfo;
extern fn shim_path_node(path: *const BtrfsPath, level: c_int) ?*ExtentBuffer;
extern fn shim_path_slot(path: *const BtrfsPath, level: c_int) c_int;
extern fn shim_path_inc_slot(path: *BtrfsPath, level: c_int) void;
extern fn shim_path_dec_slot(path: *BtrfsPath, level: c_int) void;
extern fn shim_header_nritems(eb: *const ExtentBuffer) u32;
extern fn shim_item_key_to_cpu(eb: *const ExtentBuffer, key: *BtrfsKey, nr: c_int) void;
extern fn shim_next_leaf(root: *BtrfsRoot, path: *BtrfsPath) c_int;
extern fn shim_item_size(eb: *const ExtentBuffer, slot: c_int) u32;
extern fn shim_item_ptr_offset(leaf: *ExtentBuffer, slot: c_int) c_ulong;
extern fn shim_file_extent_type(eb: *ExtentBuffer, fi: c_ulong) u8;
extern fn shim_file_extent_disk_bytenr(eb: *ExtentBuffer, fi: c_ulong) u64;
extern fn shim_file_extent_disk_num_bytes(eb: *ExtentBuffer, fi: c_ulong) u64;
extern fn shim_file_extent_offset(eb: *ExtentBuffer, fi: c_ulong) u64;
extern fn shim_file_extent_num_bytes(eb: *ExtentBuffer, fi: c_ulong) u64;
extern fn shim_file_extent_compression(eb: *ExtentBuffer, fi: c_ulong) u8;

fn isErrPtr(ptr: ?*anyopaque) bool {
	return @intFromPtr(ptr) >= @as(usize, 0) -% 4095;
}

fn lookupCsums(
	root: *BtrfsRoot,
	path: *BtrfsPath,
	start_bytenr: u64,
	total_csums: u32,
	xxhash: *u64,
) bool {
	const info = shim_root_fs_info(root);
	const csum_size: u32 = shim_fs_info_csum_size(info);
	const sectorsize: u64 = shim_fs_info_sectorsize(info);
	var pending: u32 = total_csums;
	var bytenr = start_bytenr;

	var key = BtrfsKey{
		.objectid = BTRFS_EXTENT_CSUM_OBJECTID,
		.type = BTRFS_EXTENT_CSUM_KEY,
		.offset = bytenr,
	};

	var ret = btrfs_search_slot(null, root, &key, path, 0, 0);
	if (ret < 0) return false;
	if (ret > 0) {
		if (shim_path_slot(path, 0) == 0) {
			ret = btrfs_prev_leaf(root, path);
			if (ret != 0) return false;
		} else {
			shim_path_dec_slot(path, 0);
		}
	}

	while (pending > 0) {
		const leaf = shim_path_node(path, 0) orelse return false;
		const slot = shim_path_slot(path, 0);
		if (slot >= @as(c_int, @intCast(shim_header_nritems(leaf)))) {
			ret = shim_next_leaf(root, path);
			if (ret < 0) return false;
			if (ret > 0) break;
			continue;
		}

		var found_key: BtrfsKey = undefined;
		shim_item_key_to_cpu(leaf, &found_key, slot);
		if (found_key.objectid != BTRFS_EXTENT_CSUM_OBJECTID or
			found_key.type != BTRFS_EXTENT_CSUM_KEY)
			break;

		const item_start = found_key.offset;
		const item_size = shim_item_size(leaf, slot);
		const item_end = item_start + (item_size / csum_size) * sectorsize;
		if (bytenr >= item_end or bytenr < item_start) {
			if (bytenr < item_start) break;
			shim_path_inc_slot(path, 0);
			continue;
		}

		const offset_in_item: u32 = @intCast((bytenr - item_start) / sectorsize);
		const csums_available = (item_size / csum_size) - offset_in_item;
		const csums_to_read = @min(csums_available, pending);
		const bytes_to_read = csums_to_read * csum_size;
		const ptr = shim_item_ptr_offset(leaf, slot) + offset_in_item * csum_size;

		var buf: [65536]u8 = undefined;
		if (bytes_to_read <= buf.len) {
			read_extent_buffer(leaf, &buf, ptr, bytes_to_read);
			xxhash.* = XXH64(&buf, bytes_to_read, xxhash.*);
		} else {
			// fallback: read csum by csum
			var p = ptr;
			var csum_buf: [64]u8 = undefined;
			for (0..csums_to_read) |_| {
				read_extent_buffer(leaf, &csum_buf, p, csum_size);
				xxhash.* = XXH64(&csum_buf, csum_size, xxhash.*);
				p += csum_size;
			}
		}

		pending -= csums_to_read;
		bytenr += @as(u64, csums_to_read) * sectorsize;
		shim_path_inc_slot(path, 0);
	}

	return pending == 0;
}

fn lookupExtent(
	device_info: *BtrfsFsInfo,
	ino: u64,
	subvolid: u64,
	xxhash: *u64,
) bool {
	const sectorsize: u64 = shim_fs_info_sectorsize(device_info);

	// resolve fs root for the subvolume
	const fs_root: *BtrfsRoot = blk: {
		if (subvolid == BTRFS_FS_TREE_OBJECTID)
			break :blk shim_fs_info_fs_root(device_info) orelse return false;
		var root_key = BtrfsKey{
			.objectid = subvolid,
			.type = BTRFS_ROOT_ITEM_KEY,
			.offset = @bitCast(@as(i64, -1)),
		};
		const result = btrfs_read_fs_root(device_info, &root_key);
		if (isErrPtr(@ptrCast(result))) {
			stderr.print("Error: Unable to read subvolume {d}\n", .{subvolid}) catch {};
			return false;
		}
		break :blk result orelse return false;
	};

	var key = BtrfsKey{
		.objectid = ino,
		.type = BTRFS_EXTENT_DATA_KEY,
		.offset = 0,
	};

	const path1 = btrfs_alloc_path() orelse return false;
	defer btrfs_free_path(path1);
	const path = btrfs_alloc_path() orelse return false;
	defer {
		btrfs_release_path(path);
		btrfs_free_path(path);
	}

	var ret = btrfs_search_slot(null, fs_root, &key, path, 0, 0);
	if (ret < 0) {
		stderr.print("Error: btrfs_search_slot failed with {d}\n", .{ret}) catch {};
		return false;
	}

	if (ret > 0) {
		var leaf = shim_path_node(path, 0) orelse return false;
		var slot = shim_path_slot(path, 0);
		if (slot >= @as(c_int, @intCast(shim_header_nritems(leaf)))) {
			ret = shim_next_leaf(fs_root, path);
			if (ret != 0) return false;
			leaf = shim_path_node(path, 0) orelse return false;
			slot = shim_path_slot(path, 0);
		}
		var found_key: BtrfsKey = undefined;
		shim_item_key_to_cpu(leaf, &found_key, slot);
		if (found_key.objectid != ino or found_key.type != BTRFS_EXTENT_DATA_KEY)
			return false;
	}

	// iterate file extents
	while (true) {
		const leaf = shim_path_node(path, 0) orelse return false;
		const slot = shim_path_slot(path, 0);
		if (slot >= @as(c_int, @intCast(shim_header_nritems(leaf)))) {
			ret = shim_next_leaf(fs_root, path);
			if (ret == 0) continue;
			if (ret < 0) return false;
			break;
		}

		var found_key: BtrfsKey = undefined;
		shim_item_key_to_cpu(leaf, &found_key, slot);
		if (found_key.objectid != ino or found_key.type != BTRFS_EXTENT_DATA_KEY)
			break;

		const fi = shim_item_ptr_offset(leaf, slot);
		if (shim_file_extent_type(leaf, fi) == BTRFS_FILE_EXTENT_INLINE)
			return false; // fallback to direct read

		const disk_bytenr = shim_file_extent_disk_bytenr(leaf, fi);
		if (disk_bytenr == 0) { // sparse hole
			shim_path_inc_slot(path, 0);
			continue;
		}

		const disk_num_bytes = shim_file_extent_disk_num_bytes(leaf, fi);
		const extent_offset = shim_file_extent_offset(leaf, fi);
		const num_bytes = shim_file_extent_num_bytes(leaf, fi);
		const compression = shim_file_extent_compression(leaf, fi);

		// compressed: csums cover the on-disk data
		// uncompressed: csums cover the referenced portion
		const bytenr = if (compression != 0) disk_bytenr else disk_bytenr + extent_offset;
		const total_csums: u32 = @intCast(
			(if (compression != 0) disk_num_bytes else num_bytes) / sectorsize,
		);

		const csum_root = btrfs_csum_root(device_info, bytenr) orelse return false;
		if (!lookupCsums(csum_root, path1, bytenr, total_csums, xxhash))
			return false;
		btrfs_release_path(path1);
		shim_path_inc_slot(path, 0);
	}

	return true;
}

const ResolveResult = struct {
	device_path: []const u8,
	subvolid: u64,
};

fn resolveDevice(file_path: [*:0]const u8) ?ResolveResult {
	var resolved_buf: [std.posix.PATH_MAX]u8 = undefined;
	const resolved = std.posix.realpathZ(file_path, &resolved_buf) catch {
		stderr.print("Error: realpath failed for {s}\n", .{file_path}) catch {};
		return null;
	};

	const mntinfo = std.fs.openFileAbsolute("/proc/self/mountinfo", .{}) catch {
		stderr.print("Error: Unable to open /proc/self/mountinfo\n", .{}) catch {};
		return null;
	};
	defer mntinfo.close();

	const S = struct {
		var device_path_buf: [4096]u8 = undefined;
		var device_path_len: usize = 0;
	};

	var best_prefix_len: usize = 0;
	var subvolid: u64 = BTRFS_FS_TREE_OBJECTID;
	var line_buf: [8192]u8 = undefined;
	const reader = mntinfo.reader();

	while (reader.readUntilDelimiter(&line_buf, '\n')) |line| {
		// find " - btrfs " marker, right before the fs type
		const marker = " - btrfs ";
		const marker_pos = std.mem.indexOf(u8, line, marker) orelse continue;
		const after_marker = line[marker_pos + marker.len ..];

		// extract mount_path (5th column)
		var col_iter = std.mem.tokenizeScalar(u8, line[0..marker_pos], ' ');
		var col: usize = 0;
		var mount_path: []const u8 = "";
		while (col_iter.next()) |token| {
			col += 1;
			if (col == 5) {
				mount_path = token;
				break;
			}
		}
		if (col < 5) continue;

		// check if file_path begins with mount_path
		var mp_len = mount_path.len;
		if (mp_len > 1) {
			if (!std.mem.startsWith(u8, resolved, mount_path)) continue;
			if (resolved.len > mp_len and resolved[mp_len] != '/') continue;
			mp_len += 1;
		} else {
			if (!std.mem.startsWith(u8, resolved, "/")) continue;
		}
		if (mp_len < best_prefix_len) continue;

		// extract device path (first token after "btrfs ")
		const dev_end = std.mem.indexOfScalar(u8, after_marker, ' ') orelse after_marker.len;
		const dev_path = after_marker[0..dev_end];
		@memcpy(S.device_path_buf[0..dev_path.len], dev_path);
		S.device_path_len = dev_path.len;
		best_prefix_len = mp_len;

		// parse subvolid from mount options
		const opts = if (dev_end < after_marker.len) after_marker[dev_end..] else "";
		if (std.mem.indexOf(u8, opts, "subvolid=")) |svid_pos| {
			const num_start = svid_pos + "subvolid=".len;
			var num_end = num_start;
			while (num_end < opts.len and opts[num_end] >= '0' and opts[num_end] <= '9')
				num_end += 1;
			subvolid = std.fmt.parseUnsigned(u64, opts[num_start..num_end], 10) catch
				BTRFS_FS_TREE_OBJECTID;
		}
	} else |_| {}

	if (best_prefix_len == 0) {
		stderr.print("Error: Unable to determine block device for {s}\n", .{resolved}) catch {};
		return null;
	}

	S.device_path_buf[S.device_path_len] = 0; // null-terminate for C
	return .{
		.device_path = S.device_path_buf[0..S.device_path_len],
		.subvolid = subvolid,
	};
}

fn directHash(file_path: [*:0]const u8) ?u64 {
	const file = std.fs.cwd().openFileZ(file_path, .{}) catch |err| {
		stderr.print("Error: Unable to open file {s}: {}\n", .{ file_path, err }) catch {};
		return null;
	};
	defer file.close();

	var xxhash: u64 = 0;
	var buf: [65536]u8 = undefined;
	while (true) {
		const n = file.read(&buf) catch |err| {
			stderr.print("Error: Unable to read file {s}: {}\n", .{ file_path, err }) catch {};
			return null;
		};
		if (n == 0) break;
		xxhash = XXH64(&buf, n, xxhash);
	}
	return xxhash;
}

pub fn main() !void {
	const args = try std.process.argsAlloc(std.heap.c_allocator);
	defer std.process.argsFree(std.heap.c_allocator, args);

	if (args.len != 2) {
		try stderr.print("Usage: btrfs-hash somehugefile.bin\n", .{});
		std.process.exit(255);
	}

	const file_path: [*:0]const u8 = args[1];
	const stat = std.posix.fstatat(std.posix.AT.FDCWD, std.mem.span(file_path), 0) catch {
		try stderr.print("Error: Unable to access file {s}\n", .{file_path});
		std.process.exit(255);
	};
	if (stat.mode & std.posix.S.IFMT != std.posix.S.IFREG) {
		try stderr.print("Error: {s} is not a regular file.\n", .{file_path});
		std.process.exit(255);
	}

	const resolve = resolveDevice(file_path) orelse std.process.exit(255);

	var oca = OpenCtreeArgs{
		.flags = OPEN_CTREE_PARTIAL,
		.filename = @ptrCast(resolve.device_path.ptr),
	};
	const device_info = open_ctree_fs_info(&oca) orelse {
		try stderr.print("Error: Unable to open block device {s}\n", .{resolve.device_path});
		std.process.exit(255);
	};

	var xxhash: u64 = 0;
	if (!lookupExtent(device_info, stat.ino, resolve.subvolid, &xxhash)) {
		stderr.print("Unable to lookup extent, falling back to direct read...\n", .{}) catch {};
		xxhash = directHash(file_path) orelse std.process.exit(255);
	}

	const stdout = std.io.getStdOut().writer();
	try stdout.print("{x:0>16}\n", .{xxhash});
}
