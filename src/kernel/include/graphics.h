#pragma once
#include "types.h"

#define FRAMBUFFER 0x3D000000

void put_pixel(uint32_t x, uint32_t y, Color color, framebuffer_tag* fbtag);