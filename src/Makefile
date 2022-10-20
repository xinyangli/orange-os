INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
BOOTBLOCK_TARGET := $(addprefix ../, $(BOOTBLOCK_TARGET))
BOOTLOADER_TARGET := $(addprefix ../, $(BOOTLOADER_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASFLAGS += 
OBJ = page_alloc.o
BINBOOT = boot.bin
BINLOADER = loader.bin
BIN = $(BINBOOT) $(BINLOADER)

all: kernel loader bootblock

kernel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(KERNEL_TARGET)

bootblock: $(BINBOOT)
	mv $(BINBOOT) $(BOOTBLOCK_TARGET)

loader: $(BINLOADER)
	mv $(BINLOADER) $(BOOTLOADER_TARGET)

$(OBJ):%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN):%.bin: %.asm
	$(AS) $(ASFLAGS) $< -o $@

.phony: clean

clean:
	$(RM) $(OBJ) $(BIN)
