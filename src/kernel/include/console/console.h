#pragma once

#include "../graphics/font.h"
#include "../stdlib/types.h"
#include "../drivers/keyboard.h"

enum CON_COLORS
{
    RED = 0xf02522,
    GREEN = 0x26ff67,
    BLUE = 0x1717fc,
    DODGERBLUE = 0x1E90FF,
    PURPLE = 0xd217fc,
    YELLOW = 0xfcd912,
    ORANGE = 0xfc9014,
    CYAN = 0x49f6fc,
    PINK = 0xf77cda,
    BLACK = 0x000000,
    WHITE = 0xffffff,
};

// start from KiB
#define GET_MEM_UNIT(size) ((const char*[]){"KiB", "MiB", "GiB"}[((size < 1024) ? 0 : ((size < 1024 * 1024) ? 1 : 2))]) // im sorry
#define CONVERT_MEM_UNIT(size) ( size / ((size < 1024) ? 1 : (size < 1024 * 1024 ? 1024 : (1024 * 1024))))

extern uint32_t DEFAULT_FG;
extern uint32_t DEFAULT_BG;

void init_console(uint32_t sx, uint32_t sy, uint32_t fg, uint32_t bg);
void console_end();
void console_keyboard(Key_Info info);
void set_color(uint32_t color);
void kprintf(const char *str, ...);
void goto_line_start();
void draw_cursor();
void console_loop();
void enable_input();
void clear();