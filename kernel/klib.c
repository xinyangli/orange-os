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

void disp_colint(int x, u8 col) {
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
	disp_colstr(s, col);
}