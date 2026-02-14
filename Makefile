# Makefile for btrfs-hash
MAKEFLAGS += --no-builtin-rules

BTRFS_CONFIG_TEMPLATE = $(CURDIR)/btrfs-progs-config.h
BTRFS_CONFIG_HEADER = $(CURDIR)/btrfs-progs/include/config.h
BTRFS_OBJ_DIR = $(CURDIR)/build/btrfs-progs
BTRFS_HASH_OBJ = $(CURDIR)/build/btrfs-hash.o

BTRFS_PROGS_REPO = git@github.com:kdave/btrfs-progs.git
APT_PACKAGES = clang git libudev-dev libblkid-dev uuid-dev zlib1g-dev libssl-dev
BTRFS_OBJECTS = \
	kernel-lib/list_sort.o \
	kernel-lib/raid56.o \
	kernel-lib/rbtree.o \
	kernel-lib/tables.o \
	kernel-shared/accessors.o \
	kernel-shared/async-thread.o \
	kernel-shared/backref.o \
	kernel-shared/ctree.o \
	kernel-shared/delayed-ref.o \
	kernel-shared/dir-item.o \
	kernel-shared/disk-io.o \
	kernel-shared/extent-io-tree.o \
	kernel-shared/extent-tree.o \
	kernel-shared/extent_io.o \
	kernel-shared/file-item.o \
	kernel-shared/file.o \
	kernel-shared/free-space-cache.o \
	kernel-shared/free-space-tree.o \
	kernel-shared/inode-item.o \
	kernel-shared/inode.o \
	kernel-shared/locking.o \
	kernel-shared/messages.o \
	kernel-shared/print-tree.o \
	kernel-shared/root-tree.o \
	kernel-shared/transaction.o \
	kernel-shared/tree-checker.o \
	kernel-shared/ulist.o \
	kernel-shared/uuid-tree.o \
	kernel-shared/volumes.o \
	kernel-shared/zoned.o \
	common/array.o \
	common/compat.o \
	common/cpu-utils.o \
	common/device-scan.o \
	common/device-utils.o \
	common/extent-cache.o \
	common/extent-tree-utils.o \
	common/root-tree-utils.o \
	common/filesystem-utils.o \
	common/format-output.o \
	common/fsfeatures.o \
	common/help.o \
	common/inject-error.o \
	common/messages.o \
	common/open-utils.o \
	common/parse-utils.o \
	common/path-utils.o \
	common/rbtree-utils.o \
	common/send-stream.o \
	common/send-utils.o \
	common/sort-utils.o \
	common/string-table.o \
	common/string-utils.o \
	common/sysfs-utils.o \
	common/units.o \
	common/utils.o \
	crypto/crc32c.o \
	crypto/hash.o \
	crypto/xxhash.o \
	crypto/sha224-256.o \
	crypto/blake2b-ref.o

BTRFS_SRCS = $(patsubst %.o,btrfs-progs/%.c,$(BTRFS_OBJECTS))
BTRFS_OBJS = $(patsubst %.o,$(BTRFS_OBJ_DIR)/%.o,$(BTRFS_OBJECTS))

INCLUDES = -Ibtrfs-progs -Ibtrfs-progs/include -Ibtrfs-progs/kernel-lib \
	-Ibtrfs-progs/kernel-shared -Ibtrfs-progs/common -Ibtrfs-progs/crypto \
	-Ibtrfs-progs/libbtrfs -Ibtrfs-progs/libbtrfsutil

CC = clang
CFLAGS = -Os -flto -std=gnu11 -fno-strict-aliasing $(INCLUDES) -D_GNU_SOURCE \
	-Wall -Wno-unused-parameter -include $(BTRFS_CONFIG_HEADER)
LDFLAGS = -flto
LIBS = -luuid -lblkid -ludev -lz -lm -lssl -lcrypto

all: check-deps btrfs-hash


$(BTRFS_HASH_OBJ): btrfs-hash.c $(BTRFS_CONFIG_HEADER) | build
	$(CC) $(CFLAGS) -c $< -o $@


btrfs-hash: $(BTRFS_HASH_OBJ) $(BTRFS_OBJS)
	$(CC) $(LDFLAGS) $(BTRFS_HASH_OBJ) $(BTRFS_OBJS) $(LIBS) -o $@
	strip $@
	@ls -lh $@


$(BTRFS_OBJ_DIR)/%.o: btrfs-progs/%.c $(BTRFS_CONFIG_HEADER) | $(BTRFS_OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(BTRFS_OBJ_DIR):
	mkdir -p $@


build:
	mkdir -p $@


btrfs-progs: check-deps
	@echo "Cloning btrfs-progs into $@"
	@if [ -d $@ ]; then echo "Using existing $@"; else git clone --depth 1 $(BTRFS_PROGS_REPO) $@; fi


$(BTRFS_CONFIG_HEADER): btrfs-progs $(BTRFS_CONFIG_TEMPLATE)
	mkdir -p $(dir $@)
	@if [ -f $@ ] && cmp -s $(BTRFS_CONFIG_TEMPLATE) $@; then :; else cp $(BTRFS_CONFIG_TEMPLATE) $@; fi


check-deps:
	@for pkg in $(APT_PACKAGES); do \
		if ! dpkg-query -W -f='$${Status}' $$pkg 2>/dev/null | grep -q "install ok installed"; then \
			echo "Installing required packages..."; \
			sudo apt-get update; \
			sudo apt-get install -y $(APT_PACKAGES); \
			break; \
		fi; \
	done \


clean:
	rm -f btrfs-hash $(BTRFS_HASH_OBJ)
	rm -rf $(BTRFS_OBJ_DIR)

.PHONY: all clean check-deps
