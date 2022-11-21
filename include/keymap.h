#ifndef _ORANGES_KEYMAP_H_
#define _ORANGES_KEYMAP_H_
#include "types.h"
#define MAP_COLS 3         /* Number of columns in keymap */
#define NR_SCAN_CODES 0x80 /* Number of scan codes (rows in keymap) */

#define KEY_MOD_BREAK 0x0080   /* Break Code */
#define KEY_MOD_EXT 0x0100     /* Normal function keys */
#define KEY_MOD_SHIFT_L 0x0200 /* Shift key */
#define KEY_MOD_SHIFT_R 0x0400 /* Shift key */
#define KEY_MOD_CTRL_L 0x0800  /* Control key */
#define KEY_MOD_CTRL_R 0x1000  /* Control key */
#define KEY_MOD_ALT_L 0x2000   /* Alternate key */
#define KEY_MOD_ALT_R 0x4000   /* Alternate key */

enum MODKEYS {
    /* Use values in ascii table */
    SHIFT_L = (0x08 | KEY_MOD_EXT),
    SHIFT_R, CTRL_L, CTRL_R, ALT_L, ALT_R, CAPS_LOCK,
    MODKEYS_END
};

enum EXTKEYS {
    BACKSPACE = 0x8,
    TAB = 0x9,
    ENTER = 0xA, /* Line Feed*/
    ESC = 0x1B,
    DELETE = 0x7F,
    NUM_LOCK, SCROLL_LOCK,

    F1 = (0x11 | KEY_MOD_EXT), F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    PRINTSCREEN, PAUSEBREAK, INSERT, HOME, END, PAGEUP, PAGEDOWN, UP, DOWN, LEFT, RIGHT,

    NPAD_SLASH = (0x2C | KEY_MOD_EXT), NPAD_STAR, NPAD_MINUS, NPAD_PLUS,
    NPAD_ENTER, NPAD_DOT,
    NPAD_0, NPAD_1, NPAD_2, NPAD_3, NPAD_4,
    NAPD_5, NPAD_6, NPAD_7, NPAD_8, NPAD_9,
    NPAD_UP, NPAD_DOWN, NPAD_LEFT, NPAD_RIGHT,
    NPAD_HOME, NPAD_END, NPAD_PAGEUP, NPAD_PAGEDOWN, NPAD_INS, NPAD_MID, NPAD_DEL,
};

// TODO: rotate this array
u16 keymap[NR_SCAN_CODES][MAP_COLS] = {
    /* {normal code, shift code, E0 XX} */
    {0, 0, 0},
    {ESC, ESC, 0},
    {'1', '!', 0},
    {'2', '@', 0},
    {'3', '#', 0},
    {'4', '$', 0},
    {'5', '%', 0},
    {'6', '^', 0},
    {'7', '&', 0},
    {'8', '*', 0},
    {'9', '(', 0},
    {'0', ')', 0},
    {'-', '_', 0},
    {'=', '+', 0},
    {BACKSPACE, BACKSPACE, 0},
    {TAB, TAB, 0},
    /* 0x10 */
    {'q', 'Q', 0},
    {'w', 'W', 0},
    {'e', 'E', 0},
    {'r', 'R', 0},
    {'t', 'T', 0},
    {'y', 'Y', 0},
    {'u', 'U', 0},
    {'i', 'I', 0},
    {'o', 'O', 0},
    {'p', 'P', 0},
    {'[', '{', 0},
    {']', '}', 0},
    {ENTER, ENTER, NPAD_ENTER},
    {CTRL_L, CTRL_L, CTRL_R},
    {'a', 'A', 0},
    {'s', 'S', 0},
    /* 0x20 */
    {'d', 'D', 0},
    {'f', 'F', 0},
    {'g', 'G', 0},
    {'h', 'H', 0},
    {'j', 'J', 0},
    {'k', 'K', 0},
    {'l', 'L', 0},
    {';', ':', 0},
    {'\'', '\"', 0},
    {'`', '~', 0},
    {SHIFT_L, SHIFT_L, 0},
    {'\\', '|', 0},
    {'z', 'Z', 0},
    {'x', 'X', 0},
    {'c', 'C', 0},
    {'v', 'V', 0},
    /* 0x30 */
    {'b', 'B', 0},
    {'n', 'N', 0},
    {'m', 'M', 0},
    {',', '<', 0},
    {'.', '>', 0},
    {'/', '?', NPAD_SLASH},
    {SHIFT_R, SHIFT_R, 0},
    {'*', '*', 0},
    {ALT_L, ALT_L, ALT_R},
    {' ', ' ', 0},
    {CAPS_LOCK, CAPS_LOCK, 0},
    {F1, F1, 0},
    {F2, F2, 0},
    {F3, F3, 0},
    {F4, F4, 0},
    {F5, F5, 0},
    /* 0x40 */
    {F6, F6, 0},
    {F7, F7, 0},
    {F8, F8, 0},
    {F9, F9, 0},
    {F10, F10, 0},
    {NUM_LOCK, NUM_LOCK, 0},
    {SCROLL_LOCK, SCROLL_LOCK, 0},
    {NPAD_HOME, '7', HOME},
    {NPAD_UP, '8', UP},
    {NPAD_PAGEUP, '9', PAGEUP},
    {NPAD_MINUS, '-', 0},
    {NPAD_LEFT, '4', LEFT},
    {NPAD_MID, '5', 0},
    {NPAD_RIGHT, '6', RIGHT},
    {NPAD_PLUS, '+', 0},
    {NPAD_END, '1', END},
    /* 0x50 */
    {NPAD_DOWN, '2', DOWN},
    {NPAD_PAGEDOWN, '3', PAGEDOWN},
    {NPAD_INS, '0', INSERT},
    {NPAD_DOT, '.', DELETE},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {F11, F11, 0},
    {F12, F12, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    /* 0x60 */
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    /* 0x70 */
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
};

#undef NO
#endif /* _ORANGES_KEYMAP_H_ */
