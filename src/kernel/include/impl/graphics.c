#include "../graphics.h"

void put_pixel(uint32_t x, uint32_t y, Color color, framebuffer_tag* fbtag)
{
    uint8_t* addr = (uint8_t*)(FRAMBUFFER + y * fbtag->common.framebuffer_pitch + (fbtag->common.framebuffer_bpp / 8 * x));
    
    addr[fbtag->framebuffer_red_field_position / fbtag->framebuffer_red_mask_size] = color.r;
    addr[fbtag->framebuffer_green_field_position / fbtag->framebuffer_green_mask_size] = color.g;
    addr[fbtag->framebuffer_blue_field_position / fbtag->framebuffer_blue_mask_size] = color.b;
}

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, Color color, framebuffer_tag* fbtag)
{
    for(uint32_t ry = y; ry < height; ry++)
    {
        for(uint32_t rx = x; x < width; rx++)
        {
            put_pixel(rx, ry, color, fbtag);
        }
    }
}