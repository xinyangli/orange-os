#include "klib.h"
#include "tty.h"
#include "console.h"
#include "klib.h"

void console_init(console_t *p_console, u8 nr) {
    /* vb = Video Buffer */
    const u32 vb_size = V_BUF_SIZE >> 1;
    const u32 vb_per_console = vb_size / NR_CONSOLES;
    // Base must be multiply of 2
    p_console->p_base = p_console->p_current = (achar_t *)((DISP_BASE + vb_per_console * nr) & ~1);
    p_console->screen_offset = vb_per_console * nr / 2;
    p_console->cursor = 0;
    p_console->limit = vb_per_console & ~1;
    memset(p_console->p_base, 0, p_console->limit); // Clean screen
}

void console_dispch(console_t *p_console, char ch) {
    switch(ch) {
        case '\b':
            --p_console->p_current;
            if (CONSOLE_TOP_OUT(p_console)) {
                ++p_console->p_current; return;
            }
            p_console->p_current->ch = ' ';
            p_console->p_current->attr = 0;
        case '\n':
            p_console->p_current += CONSOLE_SCREEN_WIDTH - (p_console->p_current - p_console->p_base) % CONSOLE_SCREEN_WIDTH;
            break;
        default:
            p_console->p_current->ch = ch;
            p_console->p_current->attr = 0x0F;
            p_console->p_current++;
            break;
    }
    if (CONSOLE_AT_BOTTOMLINE(p_console))
        scroll_screen(p_console, SCROLL_DOWN);
    p_console->cursor = p_console->p_current - p_console->p_base - p_console->screen_offset; 
}

void console_dispstr(console_t *p_console, const char *s) {
    while(*s != 0) {
        if (*s == '\n') {
            p_console->p_current += CONSOLE_SCREEN_WIDTH - (p_console->p_current - p_console->p_base) % CONSOLE_SCREEN_WIDTH;
        } else {
            p_console->p_current->ch = *s;
            p_console->p_current->attr = 0x0F;
            p_console->p_current++;
        }
        s++;
        if(CONSOLE_BOTTOM_OUT(p_console)) {
            p_console->p_current--; break;
        }
    }
    if (CONSOLE_AT_BOTTOMLINE(p_console))
        scroll_screen(p_console, SCROLL_DOWN);
    p_console->cursor = p_console->p_current - p_console->p_base - p_console->screen_offset; 
}

int console_printf(console_t *p_console, const char *fmt, ...) {
    char buf[BUF_SIZE];
    int c;
    void *args = (void *)&fmt + 4;
    c = vsprintf(buf, fmt, args);
    console_dispstr(p_console, buf);
    return c;
}

void console_switch(console_t *p_console) {
    flush(p_console);
}

void set_cursor(u16 cpos) {
    u8 h = ((cpos) >> 8) & 0xFF;
    u8 l = (cpos) & 0xFF;
    outb(CRT_CTRL_ATTR, 0xE);
    outb(CRT_CTRL_DATA, h);
    outb(CRT_CTRL_ATTR, 0xF);
    outb(CRT_CTRL_DATA, l);
}

void set_video_start(u32 screen_offset) {
    u8 h = ((screen_offset) >> 8) & 0xFF;
    u8 l = (screen_offset) & 0xFF;
    outb(CRT_CTRL_ATTR, 0xC);
    outb(CRT_CTRL_DATA, h);
    outb(CRT_CTRL_ATTR, 0xD);
    outb(CRT_CTRL_DATA, l);
}

void flush(console_t *p_console) {
    set_cursor(p_console->cursor);
    set_video_start(p_console->screen_offset);
    if ((p_console->p_base + p_console->cursor)->attr == 0) {
        (p_console->p_base + p_console->cursor)->attr = 0x0F;
    }
}

void scroll_screen(console_t *p_console, u8 direction) {
    switch(direction) {
        case SCROLL_UP:
            p_console->screen_offset -= CONSOLE_SCREEN_WIDTH;
            if(CONSOLE_TOP_OUT(p_console))
                p_console->screen_offset += CONSOLE_SCREEN_WIDTH;
            break;
        case SCROLL_DOWN:
            p_console->screen_offset += CONSOLE_SCREEN_WIDTH;
            if(CONSOLE_BOTTOM_OUT(p_console))
                p_console->screen_offset -= CONSOLE_SCREEN_WIDTH;
            break;
        default:
            return;
    }
}