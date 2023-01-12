#include "../graphics.h"
#include "../kmalloc.h"
#include "../stdlib.h"

uint32_t* const SRN_BUFFER = (uint32_t*) FRAMEBUFFER;
uint32_t* B_BUFFER;

void init_graphics(void)
{
    B_BUFFER = (uint32_t*)kmalloc(SCRN_WIDTH * SCRN_HEIGHT * sizeof(uint32_t));
    memset((void*)B_BUFFER, 0, SCRN_WIDTH * SCRN_HEIGHT * sizeof(uint32_t));
    SCRN_SIZE = SCRN_HEIGHT * SCRN_WIDTH * sizeof(uint32_t);
}

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


#ifndef OPTIMISE_GRAPHICS

inline void update_buffer(void)
{
    memcpy((void*)SRN_BUFFER, (void*)B_BUFFER, SCRN_SIZE);
    // memset((void*)B_BUFFER, 0, SCRN_WIDTH * SCRN_HEIGHT * sizeof(uint32_t));
}

inline void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
    B_BUFFER[y * SCRN_WIDTH + x] = color;
}

inline void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    uint32_t *buffer = B_BUFFER + y * SCRN_WIDTH;
    for(uint32_t dy = 0; dy < height; ++dy)
    {
        for(uint32_t dx = x; dx < width + x; ++dx)
        {
            buffer[dx] = color;
        }
        buffer += SCRN_WIDTH;
    }
}

inline void fill_screen(uint32_t color)
{
    for(uint32_t p = 0; p < SCRN_WIDTH * SCRN_HEIGHT; ++p)
    {
        B_BUFFER[p] = color;
    }
}
#endif