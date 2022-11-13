export BOOTBLOCK_TARGET := dbr.img
export BOOTLOADER_TARGET := loader.bin
export KERNEL_TARGET := okernel

export CC := gcc
export AS := nasm

export INCLUDE := include
export CFLAGS += -std=c11 -m32 -nostdlib -fno-stack-protector -fno-builtin -Og -mno-sse -fno-PIE -g
export LDFLAGS := -melf_i386 -g
export ASFLAGS += -g

all:
	$(MAKE) -C boot
	$(MAKE) -C kernel

bochs: all .bochsrc
	bash scripts/mkfd.sh
	bochs -q

qemu: all
	bash scripts/mkfd.sh
	qemu-system-i386 -drive file=a.img,format=raw,if=floppy -drive file=c.img,format=raw -boot a -serial mon:stdio

qemu-dbg: all
	bash scripts/mkfd.sh
	qemu-system-i386 -s -S -drive file=a.img,format=raw,if=floppy -drive file=c.img,format=raw -boot a -serial mon:stdio

.phony: clean

clean:
	$(MAKE) -C boot clean
	$(MAKE) -C kernel clean
	$(RM) $(KERNEL_TARGET) $(BOOTBLOCK_TARGET) $(BOOTLOADER_TARGET)

