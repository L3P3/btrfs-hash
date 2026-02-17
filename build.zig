const std = @import("std");

const btrfs_c_sources = [_][]const u8{
	"kernel-lib/list_sort.c",
	"kernel-lib/raid56.c",
	"kernel-lib/rbtree.c",
	"kernel-lib/tables.c",
	"kernel-shared/accessors.c",
	"kernel-shared/async-thread.c",
	"kernel-shared/backref.c",
	"kernel-shared/ctree.c",
	"kernel-shared/delayed-ref.c",
	"kernel-shared/dir-item.c",
	"kernel-shared/disk-io.c",
	"kernel-shared/extent-io-tree.c",
	"kernel-shared/extent-tree.c",
	"kernel-shared/extent_io.c",
	"kernel-shared/file-item.c",
	"kernel-shared/file.c",
	"kernel-shared/free-space-cache.c",
	"kernel-shared/free-space-tree.c",
	"kernel-shared/inode-item.c",
	"kernel-shared/inode.c",
	"kernel-shared/locking.c",
	"kernel-shared/messages.c",
	"kernel-shared/print-tree.c",
	"kernel-shared/root-tree.c",
	"kernel-shared/transaction.c",
	"kernel-shared/tree-checker.c",
	"kernel-shared/ulist.c",
	"kernel-shared/uuid-tree.c",
	"kernel-shared/volumes.c",
	"kernel-shared/zoned.c",
	"common/array.c",
	"common/compat.c",
	"common/cpu-utils.c",
	"common/device-scan.c",
	"common/device-utils.c",
	"common/extent-cache.c",
	"common/extent-tree-utils.c",
	"common/root-tree-utils.c",
	"common/filesystem-utils.c",
	"common/format-output.c",
	"common/fsfeatures.c",
	"common/help.c",
	"common/inject-error.c",
	"common/messages.c",
	"common/open-utils.c",
	"common/parse-utils.c",
	"common/path-utils.c",
	"common/rbtree-utils.c",
	"common/send-stream.c",
	"common/send-utils.c",
	"common/sort-utils.c",
	"common/string-table.c",
	"common/string-utils.c",
	"common/sysfs-utils.c",
	"common/units.c",
	"common/utils.c",
	"crypto/crc32c.c",
	"crypto/hash.c",
	"crypto/xxhash.c",
	"crypto/sha224-256.c",
	"crypto/blake2b-ref.c",
};

const btrfs_asm_sources = [_][]const u8{
	"crypto/crc32c-pcl-intel-asm_64.S",
};

const btrfsutil_c_sources = [_][]const u8{
	"libbtrfsutil/errors.c",
	"libbtrfsutil/filesystem.c",
	"libbtrfsutil/qgroup.c",
	"libbtrfsutil/stubs.c",
	"libbtrfsutil/subvolume.c",
};

pub fn build(b: *std.Build) void {
	const target = b.standardTargetOptions(.{});
	const optimize = b.standardOptimizeOption(.{});

	const exe = b.addExecutable(.{
		.name = "btrfs-hash",
		.root_source_file = b.path("btrfs-hash.zig"),
		.target = target,
		.optimize = optimize,
		.strip = true,
	});

	const btrfs_includes: []const []const u8 = &.{
		"btrfs-progs",
		"btrfs-progs/include",
		"btrfs-progs/kernel-lib",
		"btrfs-progs/kernel-shared",
		"btrfs-progs/common",
		"btrfs-progs/crypto",
		"btrfs-progs/libbtrfs",
		"btrfs-progs/libbtrfsutil",
	};

	const c_flags: []const []const u8 = &.{
		"-Os",
		"-std=gnu11",
		"-fno-strict-aliasing",
		"-D_GNU_SOURCE",
		"-Wno-unused-parameter",
		"-include",
		"btrfs-progs/include/config.h",
		"-flto",
	};

	// compile all btrfs-progs C sources with zig cc
	exe.addCSourceFiles(.{
		.files = &btrfs_c_sources,
		.root = b.path("btrfs-progs"),
		.flags = c_flags,
	});

	// compile libbtrfsutil sources
	exe.addCSourceFiles(.{
		.files = &btrfsutil_c_sources,
		.root = b.path("btrfs-progs"),
		.flags = c_flags,
	});

	// compile x86_64 CRC32 PCL assembly
	exe.addCSourceFiles(.{
		.files = &btrfs_asm_sources,
		.root = b.path("btrfs-progs"),
		.flags = &.{},
	});

	// compile shim.c
	exe.addCSourceFile(.{
		.file = b.path("shim.c"),
		.flags = c_flags,
	});

	// include paths for the C compilation
	for (btrfs_includes) |inc| {
		exe.addIncludePath(b.path(inc));
	}

	// system libraries
	exe.linkSystemLibrary("uuid");
	exe.linkSystemLibrary("blkid");
	exe.linkSystemLibrary("udev");
	exe.linkSystemLibrary("z");
	exe.linkSystemLibrary("m");
	exe.linkSystemLibrary("ssl");
	exe.linkSystemLibrary("crypto");
	exe.linkLibC();

	exe.want_lto = true;

	// Install directly into prefix root (not prefix/bin/)
	const install = b.addInstallBinFile(exe.getEmittedBin(), "../btrfs-hash");
	b.getInstallStep().dependOn(&install.step);

	const run_cmd = b.addRunArtifact(exe);
	run_cmd.step.dependOn(b.getInstallStep());
	if (b.args) |a| run_cmd.addArgs(a);
	b.step("run", "Run btrfs-hash").dependOn(&run_cmd.step);
}
