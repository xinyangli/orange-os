export INCLUDE := include
export KERNEL_TARGET := okernel

all: $(KERNEL_TARGET)

$(KERNEL_TARGET): src/Makefile
	$(MAKE) -C src

.phony: $(SUBDIR) clean

clean:
	$(MAKE) -C src clean
	$(RM) $(KERNEL_TARGET)