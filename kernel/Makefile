INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASFLAGS += $(patsubst %, -I%, $(INCLUDE))
ASM := kernel.o handlera.o kliba.o
OBJ := start.o handler.o klib.o proc.o syscall.o keyboard.o tty.o console.o \
	mm.o x86.o

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
