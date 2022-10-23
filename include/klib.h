
#ifndef ORANGE_OS_KLIB_H_
#define ORANGE_OS_KLIB_H_

#include "global.h"
#include "types.h"

static inline void itohstr(int num, char *buf) {
    for(int i = 15; i >= 0; i -= 2) {
        buf[i] = 0xF;
        buf[i-1] = num % 16;
        buf[i-1] += buf[i-1] > 9 ? 'A' - 10 : '0';
        num /= 16;
    }
    buf[16] = '$';
    buf[17] = 0xF;
}

static inline int strncmp(const char *s1, const char *s2, size_t n) {
    int i_s1 = 0, i_s2 = 0;
    while (n && s1[i_s1] != '\0' && s2[i_s2] != '\0' && s1[i_s1] == s2[i_s2]) {
        --n; ++i_s1; ++i_s2;
    }
    if (n == 0 || s1[i_s1] == s2[i_s2]) {
        /* Same str */
        return 0;
    } else {
        return -1;
    }
}

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

#endif //ORANGE_OS_KLIB_H_
