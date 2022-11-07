#include "klib.h"

int set_Ptdisp(int pos) {
    PtDisp = pos;
    return pos;
}

int disp_clear() {
    set_Ptdisp(0);
    memset((void *)0xB8000, 0, 0x8000);
    return 0;
}

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

void disp_int(int x) {
	static char s[30];
	int n = 0;
	while(x) {
		s[n++] = (x % 10) + 48;
		x /= 10;
	}
	for(int i = 0; i < n >> 1; i++) {
		char t = s[i];
		s[i] = s[n - i - 1];
		s[n - i - 1] = t;
	}
	DispStr(s);
}