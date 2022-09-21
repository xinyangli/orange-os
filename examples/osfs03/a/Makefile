BIN := pmtest1.bin
all: $(BIN) a.img

$(BIN):%.bin: %.asm
	nasm $< -o $@
	
a.img: $(BIN)
	bximage -func=create -fd=1.44M -q a.img
	dd if=$(BIN) of=a.img bs=512 count=1 conv=notrunc
	echo -ne '\x55\xaa' | dd of=a.img seek=510 bs=1 count=2 conv=notrunc

.PHONY: clean bochs qemu debug

bochs: .bochsrc a.img
	bochs -q

qemu: a.img
	qemu-system-i386 -drive file=a.img,format=raw,id=fd0 -boot a

debug: a.img
	qemu-system-x86_64 -s -S -drive file=a.img,format=raw,id=fd0 -boot a &
	gdb -ex "target remote localhost:1234" -ex "symbol $(BIN)"

clean:
	-rm a.img $(BIN)