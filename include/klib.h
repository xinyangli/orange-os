
#ifndef ORANGE_OS_KLIB_H_
#define ORANGE_OS_KLIB_H_

#include "global.h"
#include "types.h"

void DispStr(char *s);
void DispColStr(char *s, u8 col);
int set_Ptdisp(int pos);
int disp_clear();
void gain_gdt();
void apply_gdt();
void apply_idt();
void apply_tss(u16 sec);
void delay();

u32 get_hash(u8 *s, int len);
void disp_int(int x);

/* Memory and String operations */
/* Warning: No overlapping memory */
static void memcpy(void *dst, const void *src, size_t siz) {
    u8 *p_dst = dst;
    const u8 *p_src = src;
    for (size_t i = 0; i < siz; i++, p_dst++, p_src++) {
        *p_dst = *p_src;
    }
}

static void memset(void *dst, u8 c, size_t siz) {
    // TODO: Optimize performance
    for (int i = 0; i < siz; i++) {
        *(u8 *)dst = c;
    }
}

static void itohstr(int num, char *buf) {
    for (int i = 15; i >= 0; i -= 2) {
        buf[i] = 0xF;
        buf[i - 1] = num % 16;
        buf[i - 1] += buf[i - 1] > 9 ? 'A' - 10 : '0';
        num /= 16;
    }
    buf[16] = '$';
    buf[17] = 0xF;
}

static int strncmp(const char *s1, const char *s2, size_t n) {
    int i_s1 = 0, i_s2 = 0;
    while (n && s1[i_s1] != '\0' && s2[i_s2] != '\0' && s1[i_s1] == s2[i_s2]) {
        --n;
        ++i_s1;
        ++i_s2;
    }
    if (n == 0 || s1[i_s1] == s2[i_s2]) {
        /* Same str */
        return 0;
    } else {
        return -1;
    }
}

//===============================================

#endif // ORANGE_OS_KLIB_H_
