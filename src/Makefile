INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
BOOTBLOCK_TARGET := $(addprefix ../, $(BOOTBLOCK_TARGET))
BOOTLOADER_TARGET := $(addprefix ../, $(BOOTLOADER_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE)) -nostdlib -m32
ASFLAGS += $(patsubst %, -I%, $(INCLUDE))
OBJ = page_alloc.o
BINBOOT = boot.bin
BINLOADER = loader.bin

all: kernel $(BINLOADER) $(BINBOOT)

kernel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -emain -o $(KERNEL_TARGET)

$(BINBOOT): boot.asm
	$(AS) $(ASFLAGS) $< -o $@
	cp $(BINBOOT) $(BOOTBLOCK_TARGET)

$(BINLOADER): loader.asm fatheader.c loader.ld
	$(CC) $(CFLAGS) -c loader.c -o loaderc.o
	$(AS) $(ASFLAGS) -f elf $(BINLOADER:%.bin=%.asm) -o loader.o
	$(LD) -T loader.ld loader.o loaderc.o -o loader.bin
	cp $(BINLOADER) $(BOOTLOADER_TARGET)

$(OBJ):%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.phony: clean

clean:
	$(RM) $(OBJ) $(BINBOOT) $(BINLOADER) *.o
