INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE)) -s
ASFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASM := kernel.o handlera.o kliba.o
OBJ := page_alloc.o start.o handler.o klib.o global.o

all: kernel

kernel: $(OBJ)
	$(LD) $(LDFLAGS) -Ttext 0x120000 $(ASM) $(OBJ) -o $(KERNEL_TARGET)

$(OBJ):%.o: %.c $(ASM)
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM):%.o: %.asm
	$(AS) $(ASFLAGS) -f elf $< -o $@

.phony: clean

clean:
	$(RM) $(OBJ) *.o
