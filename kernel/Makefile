INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASM := kernel.o handlera.o kliba.o
OBJ := page_alloc.o start.o handler.o klib.o

all: kernel

kernel: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(ASM) -Ttext 0x30000 -o $(KERNEL_TARGET)

$(OBJ):%.o: %.c $(ASM)
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM):%.o: %.asm
	$(AS) $(ASFLAGS) -f elf $< -o $@

.phony: clean

clean:
	$(RM) $(OBJ) *.o
