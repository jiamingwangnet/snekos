#pragma once
#include "types.h"
#include "font.h"

#define FRAMEBUFFER 0x3D000000

framebuffer_tag* tagfb;
uint32_t SCRN_HEIGHT;
uint32_t SCRN_WIDTH;

void put_pixel(uint32_t x, uint32_t y, Color color, framebuffer_tag* fbtag);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color, framebuffer_tag* fbtag);