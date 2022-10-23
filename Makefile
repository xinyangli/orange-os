export BOOTBLOCK_TARGET := dbr.img
export BOOTLOADER_TARGET := loader.bin
export KERNEL_TARGET := okernel

export CC := i586-suse-linux-gcc
export AS := nasm

export INCLUDE := include
export CFLAGS += --std=c11 -m32 -nostdinc -fno-stack-protector -fno-builtin -O2 -mno-sse

all:
	$(MAKE) -C src

bochs: all .bochsrc
	scripts/mkfd.sh
	bochs -q

.phony: clean

clean:
	$(MAKE) -C src clean
	$(RM) $(KERNEL_TARGET) $(BOOTBLOCK_TARGET) $(BOOTBLOCK_TARGET)
