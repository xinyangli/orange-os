INCLUDE := $(addprefix ../, $(INCLUDE))
BOOTBLOCK_TARGET := $(addprefix ../, $(BOOTBLOCK_TARGET))
BOOTLOADER_TARGET := $(addprefix ../, $(BOOTLOADER_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASFLAGS += $(patsubst %, -I%, $(INCLUDE))
OBJ = page_alloc.o
BINBOOT = boot.bin
BINLOADER = loader.bin

all: $(BINLOADER) $(BINBOOT)

$(BINBOOT): boot.asm
	$(AS) $(ASFLAGS) $< -o $@
	cp $(BINBOOT) $(BOOTBLOCK_TARGET)

$(BINLOADER): loader.asm loader.c loader.ld
	$(CC) $(CFLAGS) -c loader.c -o loaderc.o
	$(AS) $(ASFLAGS) -f elf $(BINLOADER:%.bin=%.asm) -o loader.o
	$(LD) $(LDFLAGS) -T loader.ld loader.o loaderc.o -o loader.bin
	cp $(BINLOADER) $(BOOTLOADER_TARGET)

.phony: clean

clean:
	$(RM) $(OBJ) $(BINBOOT) $(BINLOADER) *.o
