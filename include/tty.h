#ifndef ORANGE_OS_TTY_H
#define ORANGE_OS_TTY_H

#include "kqueue.h"
#include "console.h"

#define NR_TTY 3

#define CRT_CTRL_ATTR 0x3D4
#define CRT_CTRL_DATA 0x3D5

#define V_BUF_SIZE 0x8000

typedef struct {
    kqueue buf; /* u16 queue, input buffer for each tty */
    console_t console;
} tty_t;

void tty_init(tty_t *ttys);
void task_tty();
void tty_write();

extern tty_t ttys[NR_TTY];

#endif // ORANGE_OS_TTY_H