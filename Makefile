export BOOTBLOCK_TARGET := dbr.img
export BOOTLOADER_TARGET := loader.bin
export KERNEL_TARGET := okernel

export CC := gcc
export AS := nasm

export INCLUDE := include
export CFLAGS += --std=c11 -m32 -nostdlib -fno-pie -fno-stack-protector -fno-builtin -Og -mno-sse
export LDFLAGS := -melf_i386

all:
	$(MAKE) -C load
	$(MAKE) -C kernel

bochs: all .bochsrc
	scripts/mkfd.sh
	bochs -q

.phony: clean

clean:
	$(MAKE) -C load clean
	$(MAKE) -C kernel clean
	$(RM) $(KERNEL_TARGET) $(BOOTBLOCK_TARGET) $(BOOTBLOCK_TARGET)
