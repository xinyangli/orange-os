#ifndef _ORANGES_KEYBOARD_H_
#define _ORANGES_KEYBOARD_H_
#include "kqueue.h"

#define IS_VISIBLE(c) ((c) > 0x20 && (c) < 0x7F)
#define IS_ASCII(c) ((c) > 0x20 && (c) < 0x7F)
#define IS_LOWERCASE(c) ((c) >= 'a' && (c) <= 'z')
#define IS_CAPITAL(c) ((c) >= 'A' && (c) <= 'Z')
#define IS_LETTER(c) (IS_LOWERCASE(c) || IS_CAPITAL(c))
#define IS_RELEASE(c) (((c) & KEY_MOD_BREAK) >> 7)

#define IS_SHIFTED(flag) ((flag) & (KEY_MOD_SHIFT_L | KEY_MOD_SHIFT_R))

extern u8 buf[64];
extern kqueue kb_queue;
extern kqueue *p_kb_queue;

int keyboard_read();

#endif /* _ORANGES_KEYBOARD_H_ */
