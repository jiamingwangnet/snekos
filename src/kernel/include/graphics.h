#pragma once
#include "types.h"
#include "font.h"

#define FRAMEBUFFER 0x3D000000

framebuffer_tag* tagfb;
uint32_t SCRN_HEIGHT;
uint32_t SCRN_WIDTH;

void init_graphics(void);

uint32_t init_color(Color color);
uint32_t init_color_u32(uint32_t color);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void update_buffer(void);