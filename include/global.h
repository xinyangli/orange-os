#ifndef ORANGE_OS_GLOBAL_H
#define ORANGE_OS_GLOBAL_H

#include "protect.h"
#include "types.h"

#define GDT_SIZE 128
#define IDT_SIZE 256

extern int PtDisp;
extern u8 gdt_ptr[6];
extern DESCRIPTOR gdt[GDT_SIZE];
extern u8 idt_ptr[6];
extern GATE idt[IDT_SIZE];

#endif //ORANGE_OS_GLOBAL_H