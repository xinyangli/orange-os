#ifndef ORANGE_OS_GLOBAL_H
#define ORANGE_OS_GLOBAL_H

#include "protect.h"
#include "types.h"

#define GDT_SIZE 128
#define IDT_SIZE 256

int PtDisp;
u8 gdt_ptr[6];
DESCRIPTOR gdt[GDT_SIZE];
u8 idt_ptr[6];
GATE idt[IDT_SIZE];

#endif //ORANGE_OS_GLOBAL_H