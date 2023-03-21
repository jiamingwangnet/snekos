#pragma once
#include "../stdlib/types.h"
#include "font.h"

uint64_t FRAMEBUFFER;

framebuffer_tag* tagfb;
uint64_t SCRN_HEIGHT;
uint64_t SCRN_WIDTH;
size_t SCRN_SIZE;

uint32_t init_color(Color color);
uint32_t init_color_u32(uint32_t color);

void init_graphics(void);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void fill_screen(uint32_t color);
void update_buffer(void);

#ifdef OPTIMISE_GRAPHICS_C
void draw_rect_aligned(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
#endif