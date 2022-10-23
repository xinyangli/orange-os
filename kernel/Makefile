INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE)) -s
ASFLAGS += $(patsubst %, -I%, $(INCLUDE))
LDFLAGS += -s
ASM := kernel.o handlera.o kliba.o
OBJ := page_alloc.o start.o handler.o klib.o global.o

all: kernel

kernel: $(OBJ)
	$(CC) $(CFLAGS) -Ttext 0x120000 $(OBJ) $(ASM) -o $(KERNEL_TARGET)

$(OBJ):%.o: %.c $(ASM)
	$(CC) $(CFLAGS) -c $< -o $@

$(ASM):%.o: %.asm
	$(AS) $(ASFLAGS) -f elf $< -o $@

.phony: clean

clean:
	$(RM) $(OBJ) *.o
