export BOOTBLOCK_TARGET := dbr.img
export BOOTLOADER_TARGET := loader.bin
export KERNEL_TARGET := okernel

export CC := i586-suse-linux-gcc
export AS := nasm

export INCLUDE := include
export CFLAGS += --std=c11 -m32 -nostdinc -O2 -mno-sse

all:
	$(MAKE) -C src

.phony: clean

clean:
	$(MAKE) -C src clean
	$(RM) $(KERNEL_TARGET) $(BOOTBLOCK_TARGET) $(BOOTBLOCK_TARGET)
