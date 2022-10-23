#!/bin/bash
# Make FAT12 floppy drive and substitute 0 sector with dbr.img
# and copy loader.bin to it

BINLOADER=loader.bin
BINBOOTBLOCK=dbr.img
TARGET_IMG=a.img
DISK_IMG=c.img

dd if=/dev/zero of=$TARGET_IMG bs=1K count=1440
sudo mkfs.fat -F12 $TARGET_IMG
dd if=$BINBOOTBLOCK of=$TARGET_IMG bs=512 count=1 conv=notrunc
mkdir -p .mnt
sudo mount -t msdos -o loop $TARGET_IMG .mnt
sudo cp $BINLOADER .mnt/LOADER.BIN
sudo umount .mnt

rm -f $DISK_IMG
bximage -func=create -hd=10M -imgmode=flat -sectsize=512 -q $DISK_IMG
sudo mkfs.fat -F32 $DISK_IMG
sudo mount -o loop $DISK_IMG .mnt
sudo cp okernel .mnt/OKERNEL

sudo umount .mnt
rmdir .mnt