# btrfs-hash

Hash huge files on btrfs for fast identification.

## Usage

```bash
sudo ./btrfs-hash somehugefile.bin
```

## How It Works

1. Resolves the file's btrfs block device
2. Opens the btrfs filesystem metadata (that is why it needs sudo!)
3. Looks up the file's extent data and stored checksums
4. Hashes all checksums into a single xxhash
5. Small inlined files are hashed directly

> [!WARNING]
> This tool is not made for security purposes and may return different hashes on different devices.

## Building

On Debian, do this:

```bash
git clone https://github.com/L3P3/btrfs-hash.git
cd btrfs-hash
make
```

The result is the executable `btrfs-hash`.

## License

GPL v2, as this is based on btrfs-progs.

## Credits

Heavily inspired by [dduper](https://github.com/Lakshmipathi/dduper).

Which is also building on [btrfs-progs](https://github.com/kdave/btrfs-progs).

And Copilot helped me a lot.
