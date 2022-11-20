#include "../graphics.h"

Color* const SRN_BUFFER = (Color*) FRAMEBUFFER;

void put_pixel(uint32_t x, uint32_t y, Color color, framebuffer_tag* fbtag)
{
    SRN_BUFFER[y * SCRN_WIDTH + x] = color;
}

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color, framebuffer_tag* fbtag)
{
    for(uint32_t ry = y; ry < height + y; ry++)
    {
        for(uint32_t rx = x; rx < width + x; rx++)
        {
            put_pixel(rx, ry, color, fbtag);
        }
    }
}