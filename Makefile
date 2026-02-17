# Makefile for btrfs-hash

BTRFS_CONFIG_TEMPLATE = $(CURDIR)/btrfs-progs-config.h
BTRFS_CONFIG_HEADER = $(CURDIR)/btrfs-progs/include/config.h
BTRFS_PROGS_REPO = git@github.com:kdave/btrfs-progs.git
APT_PACKAGES = git libudev-dev libblkid-dev uuid-dev zlib1g-dev libssl-dev libbtrfs-dev

all: btrfs-hash

btrfs-hash: btrfs-hash.zig build.zig shim.c $(BTRFS_CONFIG_HEADER)
	zig build -Doptimize=ReleaseSmall -p .
	@ls -lh $@

btrfs-progs:
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
	rm -f btrfs-hash
	rm -rf .zig-cache

.PHONY: all clean check-deps
