#include "klib.h"

void delay(int time)
{
	int i, j, k;
	for(k=0;k<time;k++){
		/*for(i=0;i<10000;i++){	for Virtual PC	*/
		for(i=0;i<10;i++){/*	for Bochs	*/
			for(j=0;j<10000;j++){}
		}
	}
}

u32 get_hash(u8 *s, int len) {
	u32 res = 0;
	for(int i = 0; i < len; i++) {
		res = (res << 25) ^ (res >> 7) ^ s[i];
		res = res % 998244353;
	}
	return res;
}

#define DISP_BASE 0xB8000
#define DISP_END 0xB8FA0
#define DISP_SIZE 0xFA0

int set_Ptdisp(int pos) {
    PtDisp = pos;
    return pos;
}

int disp_clear() {
    set_Ptdisp(0);
    memset((void *)DISP_BASE, 0, 0x8000);
    return 0;
}

int disp_upline() {
	memcpy((void *)DISP_BASE, (void *)(DISP_BASE + 160), DISP_SIZE - 160);
	memset((void *)(DISP_END - 160), 0, 160);
	return 0;
}

void disp_str(char *s) {
	u8* gs = (u8 *)DISP_BASE;
	while(*s != 0) {
		if(DISP_BASE + PtDisp == DISP_END) {
			disp_upline();
			set_Ptdisp(PtDisp - 160);
		}
		if(*s == '\n') {
			PtDisp = (PtDisp / 160 + 1) * 160;
			s++;
			continue;
		}
		gs[PtDisp++] = *s;
		gs[PtDisp++] = 0x0F;
		s++;
	}
}

void disp_colstr(char *s, u8 col) {
	u8* gs = (u8 *)DISP_BASE;
	while(*s != 0) {
		if(DISP_BASE + PtDisp == DISP_END) {
			disp_upline();
			set_Ptdisp(PtDisp - 160);
		}
		if(*s == '\n') {
			PtDisp = (PtDisp / 160 + 1) * 160;
			s++;
			continue;
		}
		gs[PtDisp++] = *s;
		gs[PtDisp++] = col;
		s++;
	}
}

void disp_int(int x) {
	static char s[30];
	int n = 0;
	while(x) {
		s[n++] = (x % 10) + 48;
		x /= 10;
	}
	if(n == 0) s[n++] = 48;
	s[n] = 0;
	for(int i = 0; i < n >> 1; i++) {
		char t = s[i];
		s[i] = s[n - i - 1];
		s[n - i - 1] = t;
	}
	disp_str(s);
}

// support: %d %x %s %c
// %.8x extend with '0' 
// %8x extend with space
int parse_fmt(const char *fmt, fmt_block_t *fb) {
    int vtype = 0, exn = 0, extype = 0;
    const char *pfmt = fmt + 1;
    // 0 extend
    if(*pfmt == '.') extype = EXT_ZERO, pfmt++;
    else extype = EXT_BLANK;
    // get extend number
    while(*pfmt >= '0' && *pfmt <= '9') {
        exn = exn * 10 + (*pfmt - 48);
        pfmt++;
    }
    // get type
    switch(*pfmt) {
        case 'd': vtype = FMT_INT32; pfmt++; break;
        case 'x': vtype = FMT_HEX32; pfmt++; break;
        case 's': vtype = FMT_STR; pfmt++; break;
        case 'c': vtype = FMT_CHAR; pfmt++; break;
        default: return -1;
    }
    // set
    fb->val_type = vtype;
    fb->ext_num = exn;
    fb->ext_type = extype;
    return pfmt - fmt;
}

int apply_fmt(char *dst, void *arg, fmt_block_t *fb) {
    char tmp[1 << 8], *pt = tmp;
    // char
    if(fb->val_type == FMT_CHAR) {
        char ch = *(char *)arg;
        *dst = ch;
        return 1;
    }
    // string
    if(fb->val_type == FMT_STR) {
        const char *src = *(void **)arg;
        while(*src != 0) *pt++ = *src++;
    }
    // int -> string
    if(fb->val_type == FMT_INT32) {
        int x = *(int *)arg;
        int n = 0, f = x < 0 ? 1 : 0;
        if(f) x = -x, tmp[n++] = '-';
        if(x == 0) tmp[n++] = 0;
        while(x) {
            tmp[n++] = x % 10 + '0';
            x /= 10;
        }
        // rev
        for(int i = f, j = n - i - (f ^ 1); i < n >> 1; i++, j--) {
            char t = tmp[i];
            tmp[i] = tmp[j];
            tmp[j] = t;
        }
        pt = tmp + n;
    }
    // hex -> string
    if(fb->val_type == FMT_HEX32) {
        u32 x = *(u32 *)arg;
        int fir = 0, n = 0;
        for(int i = 28; i >= 0; i -= 4) {
            u8 cur = (x >> i) & 0xFU;
            if(cur != 0) fir = 1;
            if(fir) tmp[n++] = cur < 10 ? cur + '0' : cur - 10 + 'A';
        }
        if(!fir) tmp[n++] = 0;
        pt = tmp + n;
    }
    // apply
    int len = pt - tmp;
    if(fb->ext_num == 0) memcpy(dst, tmp, len);
    else if(fb->ext_num <= len) {
        len = fb->ext_num;
        memcpy(dst, tmp, len);
    } else {
        int exl = fb->ext_num - len;
        int exc = fb->ext_type == EXT_ZERO ? '0' : ' ';
        memset(dst, exc, exl);
        memcpy(dst + exl, tmp, len);
        len += exl;
    }
    return len;
}

int vsprintf(char *buf, const char *fmt, void *args) {
    char *p = buf;
    void *arg = args;
    fmt_block_t *fb;
    while(*fmt != 0) {
        if(*fmt == '%') {
            if(fmt[1] == '%') *p++ = '%', fmt += 2;
            else {
                int len = parse_fmt(fmt, fb);
                // Unknown form
                if(len == -1) {
                    *p++ = *fmt++;
                    continue;
                }
                p += apply_fmt(p, arg, fb);
                fmt += len;
                arg += 4;
            }
        } else *p++ = *fmt++;
    }
    *p = 0;
    return p - buf;
}

int printf(const char *fmt, ...) {
    char buf[1 << 10];
    int c;
    void *args = (void *)&fmt + 4;
    c = vsprintf(buf, fmt, args);
    disp_str(buf);
    return c;
}