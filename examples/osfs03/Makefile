INCLUDE := .
BIN := ex1.com
ASMFLAGS := $(patsubst %, -I%, $(INCLUDE)) $(ASMFLAGS)
all: $(BIN) a.img

$(BIN):%.com: %.asm
	nasm $(ASMFLAGS) $< -o $@
	
cpfile: $(BIN) a.img
	mkdir -p mnt
	guestmount --format=raw -a a.img -m /dev/sda --rw mnt
	cp $(BIN) mnt
	guestunmount mnt
	rmdir mnt

.PHONY: clean bochs qemu debug

a.img:
	bximage -func=create -fd=1.44M -q a.img
	mkfs.fat a.img

bochs: .bochsrc cpfile
	bochs -q

qemu: cpfile
	qemu-system-i386 -drive file=a.img,format=raw,index=1,if=floppy -drive file=../freedos.img,format=raw,index=0,if=floppy -boot a -display curses -serial mon:stdio

debug: cpfile $(BIN)
	qemu-system-x86_64 -s -S -drive file=a.img,format=raw,id=fd0 -boot a &

clean:
	-rm a.img $(BIN)
