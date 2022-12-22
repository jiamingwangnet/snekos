#include "../graphics.h"

uint32_t* const SRN_BUFFER = (uint32_t*) FRAMEBUFFER;

inline uint32_t init_color(Color color)
{
    return  ((uint32_t) color.r << tagfb->framebuffer_red_field_position) |
            ((uint32_t) color.g << tagfb->framebuffer_green_field_position) |
            ((uint32_t) color.b << tagfb->framebuffer_blue_field_position);
}

inline uint32_t init_color_u32(uint32_t color)
{
    return  ((color >> 16 & 0xff) << tagfb->framebuffer_red_field_position) |
            ((color >> 8  & 0xff) << tagfb->framebuffer_green_field_position) |
            ((color       & 0xff) << tagfb->framebuffer_blue_field_position);
}

inline void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    SRN_BUFFER[y * SCRN_WIDTH + x] = color;
}

inline void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    for(uint32_t ry = y; ry < height + y; ry++)
    {
        for(uint32_t rx = x; rx < width + x; rx++)
        {
            put_pixel(rx, ry, color);
        }
    }
}