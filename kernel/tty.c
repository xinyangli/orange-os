#include "tty.h"
#include "keyboard.h"
#include "klib.h"
#include "kqueue.h"
#include "console.h"

tty_t ttys[NR_TTY] = {{.buf = KQUEUE_INIT(0, 0)}};
u8 tty_bufs[3][64];

void tty_init(tty_t *ttys) {
    for (int i = 0; i < NR_TTY; i++) {
        ttys[i].buf.buf = tty_bufs[i];
        ttys[i].buf.tol = 64;
    }
}

void task_tty() {
    tty_t *p_fgtty = ttys; /* tty in the foreground */
    char input[2];
    while (1) {
        if (keyboard_read(&p_fgtty->buf)) {
            u16 keystroke = *(u16 *)kqueue_front(&p_fgtty->buf, sizeof(u16));
            kqueue_pop(&p_fgtty->buf);
            console_dispch(&p_fgtty->console, keystroke & 0x7F);
        }
        // for (tty_t *p_tty = ttys; p_tty < ttys + sizeof(ttys); p_tty++) {
        //     /* Other process may access tty buffer at the same time */
        //     /* Turn off task switch */
        //     cli();
        //     while (!kqueue_empty(&p_tty->buf)) {
        //         u16 keystroke = *(u16 *)kqueue_front(&p_fgtty->buf, sizeof(u16));
        //         kqueue_pop(&p_fgtty->buf);
                
        //     }
        //     sti();
        // }
        
        delay(5);
    }
}

void tty_write() {
    return;
}