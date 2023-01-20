#pragma once

#include "font.h"
#include "types.h"
#include "keyboard.h"

void init_console(uint32_t sx, uint32_t sy, uint32_t fg, uint32_t bg);
void console_end();
void console_keyboard(Key_Info info);
void kprintch(char c);
void kprintf(const char *str);
void draw_cursor();
void console_loop();
void printcmd();
void clear();