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