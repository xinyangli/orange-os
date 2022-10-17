INCLUDE := $(addprefix ../, $(INCLUDE))
KERNEL_TARGET := $(addprefix ../, $(KERNEL_TARGET))
CFLAGS += $(patsubst %, -I%, $(INCLUDE))
OBJ = page_alloc.o

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(KERNEL_TARGET)

$(OBJ):%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.phony: clean

clean:
	- $(RM) $(OBJ)
