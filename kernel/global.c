#include "types.h"
#include "protect.h"
#include "global.h"

int PtDisp;
u8 gdt_ptr[6];
DESCRIPTOR gdt[GDT_SIZE];
u8 idt_ptr[6];
GATE idt[IDT_SIZE];
