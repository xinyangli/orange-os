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