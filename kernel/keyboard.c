#include "keyboard.h"
#include "klib.h"
#include "types.h"
#include "kqueue.h"
#include "keymap.h"

u8 buf[64];
kqueue kb_queue = KQUEUE_INIT(buf, 64);
kqueue *p_kb_queue = &kb_queue;

struct {
    u8 ctrl_l : 1;
    u8 ctrl_r : 1;
    u8 shift_l : 1;
    u8 shift_r : 1;
    u8 alt_l : 1;
    u8 alt_r : 1;
    u8 caps : 1;
} kb_state;

static inline int update_state(u16 keystroke, u8 scan_code) {
    switch(keystroke) {
        case CTRL_L:
            kb_state.ctrl_l = !IS_RELEASE(scan_code);
            break;
        case CTRL_R:
            kb_state.ctrl_r = !IS_RELEASE(scan_code);
            break;
        case SHIFT_L:
            kb_state.shift_l = !IS_RELEASE(scan_code);
            break;
        case SHIFT_R:
            kb_state.shift_r = !IS_RELEASE(scan_code);
            break;
        case ALT_L:
            kb_state.alt_l = !IS_RELEASE(scan_code);
            break;
        case ALT_R:
            kb_state.alt_r = !IS_RELEASE(scan_code);
            break;
        case CAPS_LOCK:
            kb_state.caps ^= !IS_RELEASE(scan_code);
            break;
        default:
            return 0;
    }
    return 1;
}

int keyboard_read(kqueue *kq_buf) {
    u8 scan_code;
    u16 keystroke = 0;   /* Parsed key code, ascii number or defined in keyboard.h */
    u8 col_modifier = 0;
    char input[2];

    u8 flags = 0;
    u8 buf_appended = 0; /* Whether tty buffer is appended with character*/
    while(!kqueue_empty(p_kb_queue)) {
        scan_code = *(u8 *)kqueue_front(p_kb_queue, sizeof(u8));
        // TODO: Maybe use lock to control?
        kqueue_pop(p_kb_queue);

        if (kb_state.shift_l || kb_state.shift_r)
            keystroke = keymap[scan_code & 0x7F][1];
        else
            keystroke = keymap[scan_code & 0x7F][0];

        if (update_state(keystroke, scan_code)) {
            continue;
        }

        if (IS_RELEASE(scan_code)) {
            continue;
        }

        buf_appended = 1;

        // TODO: 0xE0 xx

        if (kb_state.caps) {
            if (IS_LOWERCASE(keystroke & 0x7F))
                keystroke = keystroke - 'a' + 'A';
            else if (IS_CAPITAL(keystroke & 0x7F) &&
                     (kb_state.shift_l || kb_state.shift_r))
                keystroke = keystroke - 'A' + 'a';
        }
        // TODO: Is buffer full?
        kqueue_push(kq_buf, &keystroke, sizeof(u16));
        // disp_int(scan_code);
        // disp_str(" ");
    }
    return buf_appended;
}