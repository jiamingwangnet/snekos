#pragma once

#include "font.h"
#include "types.h"
#include "keyboard.h"

void init_console(uint32_t sx, uint32_t sy, Color fg, Color bg, framebuffer_tag* fb);
void shell_keyboard(Key_Info info);
void shell_draw(char c);
void draw_cursor();