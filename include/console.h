#ifndef ORANGE_OS_CONSOLE_H
#define ORANGE_OS_CONSOLE_H
#include "types.h"
#define NR_CONSOLES 3

#define DISP_BASE 0xB8000
#define DISP_END 0xB8FA0
#define DISP_SIZE 0xFA0
#define CONSOLE_SCREEN_WIDTH 80
#define CONSOLE_SCREEN_HEIGHT 25
#define CONSOLE_AT_TOPLINE(p_con)                                                \
    ((u32)p_con->p_current - 2 * p_con->screen_offset - DISP_BASE < CONSOLE_SCREEN_WIDTH)
#define CONSOLE_AT_BOTTOMLINE(p_con)                                             \
    ((u32)p_con->p_current - 2 * p_con->screen_offset - DISP_BASE >= CONSOLE_SCREEN_WIDTH * CONSOLE_SCREEN_HEIGHT)
#define CONSOLE_TOP_OUT(p_con) \
    (p_con->p_current < p_con->p_base)
#define CONSOLE_BOTTOM_OUT(p_con) \
    (p_con->p_current >= p_con->p_base + p_con->limit - 1)

enum SCROLL_DIRECT { SCROLL_UP, SCROLL_DOWN };

/* Char with attribute */
typedef struct {
    char ch;
    u8 attr;
} achar_t;

typedef struct {
    achar_t *p_base;
    achar_t *p_current;
    u32 screen_offset;
    u32 limit;
    u16 cursor;
} console_t;

void console_init(console_t *p_console, u8 nr);
void set_cursor(u16 cpos);
void set_video_start(u32 offset);
void scroll_screen(console_t *, u8);
void flush(console_t *);

void console_dispch(console_t *p_console, char ch);
void console_dispstr(console_t *p_console, const char *s);
void console_switch(console_t *p_console);

#endif // ORANGE_OS_CONSOLE_H