all: .bochsrc a.img
	bochs -q

boot.bin: boot.asm
	nasm boot.asm -o boot.bin
	
a.img: boot.bin
	bximage -func=create -fd=1.44M -q a.img
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc

.PHONY: clean

clean:
	rm a.img boot.bin