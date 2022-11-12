INCLUDE := $(addprefix ../, $(INCLUDE))
BOOTBLOCK_TARGET := $(addprefix ../, $(BOOTBLOCK_TARGET))
BOOTLOADER_TARGET := $(addprefix ../, $(BOOTLOADER_TARGET))
ASFLAGS := 
CFLAGS += $(patsubst %, -I%, $(INCLUDE))
BINBOOT = boot.bin
BINLOADER = loader.bin

all: $(BINLOADER) $(BINBOOT)

$(BINBOOT): boot.asm
	$(AS) $(ASFLAGS) $< -o $@
	cp $(BINBOOT) $(BOOTBLOCK_TARGET)

$(BINLOADER): loader.asm loader.c loader.ld
	$(CC) $(CFLAGS) -c loader.c -o loaderc.o
	$(AS) $(ASFLAGS) -f elf $(BINLOADER:%.bin=%.asm) -o loader.o
	$(LD) $(LDFLAGS) -z noexecstack -T loader.ld loader.o loaderc.o -o loader.bin
	cp $(BINLOADER) $(BOOTLOADER_TARGET)

.phony: clean

clean:
	$(RM) $(BINBOOT) $(BINLOADER) *.o
