#include "../graphics.h"

void put_pixel(uint32_t x, uint32_t y, Color color, framebuffer_tag* fbtag)
{
    uint8_t* addr = (uint8_t*)(FRAMBUFFER + y * fbtag->common.framebuffer_pitch + x);
    addr[0] = color.b;
    addr[1] = color.g;
    addr[2] = color.r;
}