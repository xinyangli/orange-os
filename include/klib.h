
#ifndef ORANGE_OS_KLIB_H_
#define ORANGE_OS_KLIB_H_

#include "global.h"
#include "types.h"

void DispStr(char *s);
void DispColStr(char *s, u8 col);
void out_byte(u16 port, u8 byte);
void in_byte(u16 port);
void* memcpy(void *dst, void *src, size_t siz);
void* memset(void *dst, u8 val, size_t siz);
int set_Ptdisp(int pos);
int disp_clear();
void gain_gdt();
void apply_gdt();
void apply_idt();
void apply_tss(u16 sec);
void delay();

u32 get_hash(u8 *s, int len);
void disp_int(int x);

#endif //ORANGE_OS_KLIB_H_
