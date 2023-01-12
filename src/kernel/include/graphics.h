#pragma once
#include "types.h"
#include "font.h"

#define FRAMEBUFFER 0x3D000000

framebuffer_tag* tagfb;
uint32_t SCRN_HEIGHT;
uint32_t SCRN_WIDTH;
size_t SCRN_SIZE;

uint32_t init_color(Color color);
uint32_t init_color_u32(uint32_t color);

#ifdef OPTIMISE_GRAPHICS
extern void init_graphics(void);
extern void put_pixel(uint32_t x, uint32_t y, uint32_t color);
extern void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
extern void fill_screen(uint32_t color);
#else
void init_graphics(void);
void put_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void fill_screen(uint32_t color);
#endif

void update_buffer(void);