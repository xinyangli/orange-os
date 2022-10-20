#!/bin/bash
# Make FAT12 floppy drive and substitute 0 sector with dbr.img
# and copy loader.bin to it

BINLOADER=loader.bin
BINBOOTBLOCK=dbr.img
TARGET_IMG=a.img

dd if=/dev/zero of=$TARGET_IMG bs=1K count=1440
sudo mkfs.fat -F12 $TARGET_IMG
dd if=$BINBOOTBLOCK of=$TARGET_IMG bs=512 count=1 conv=notrunc
mkdir -p .mnt
sudo mount -t msdos -o loop $TARGET_IMG .mnt
sudo cp $BINLOADER .mnt/LOADER.BIN
sudo umount .mnt
rmdir .mnt
