#include "../include/graphics/graphics.h"
#include "../include/memory/kmalloc.h"
#include "../include/stdlib/stdlib.h"

uint32_t* const SRN_BUFFER = (uint32_t*) FRAMEBUFFER;
uint32_t* B_BUFFER;
uint32_t SCRN_PITCH;

void init_graphics(void)
{
    // temporary fix for overwriting multiboot2 stuff
    framebuffer_tag *fbptr = (framebuffer_tag*)kmalloc(sizeof(framebuffer_tag));
    memcpy((void*)fbptr, (void*)tagfb, sizeof(framebuffer_tag));
    tagfb = fbptr;

    B_BUFFER = (uint32_t*)kmalloc(SCRN_WIDTH * SCRN_HEIGHT * sizeof(uint32_t));
    memset((void*)B_BUFFER, 0, SCRN_WIDTH * SCRN_HEIGHT * sizeof(uint32_t));
    SCRN_SIZE = SCRN_HEIGHT * SCRN_WIDTH * sizeof(uint32_t);
    SCRN_PITCH = tagfb->common.framebuffer_pitch;

    #ifdef DEBUG_LOG
            serial_str("framebuffer address: 0x");
            char fbaddr[15];
            itoa((unsigned long)tagfb->common.framebuffer_addr, fbaddr, 16);
            serial_str(fbaddr);
            serial_char('\n');

            serial_str("width: ");
            char fbwidth[15];
            itoa(tagfb->common.framebuffer_width, fbwidth, 10);
            serial_str(fbwidth);
            serial_char('\n');

            serial_str("height: ");
            char fbheight[15];
            itoa(tagfb->common.framebuffer_height, fbheight, 10);
            serial_str(fbheight);
            serial_char('\n');

            serial_str("bpp: ");
            char fbbpp[15];
            itoa(tagfb->common.framebuffer_bpp, fbbpp, 10);
            serial_str(fbbpp);
            serial_char('\n');

            serial_str("pitch: ");
            char fbptch[15];
            itoa(tagfb->common.framebuffer_pitch, fbptch, 10);
            serial_str(fbptch);
            serial_char('\n');

            serial_str("fb type: ");
            char fbtype[15];
            itoa(tagfb->common.framebuffer_type, fbtype, 10);
            serial_str(fbtype);
            serial_char('\n');

            serial_str("red mask size: ");
            char rmsize[15];
            itoa(tagfb->framebuffer_red_mask_size, rmsize, 10);
            serial_str(rmsize);
            serial_char('\n');

            serial_str("red field pos: ");
            char rfp[15];
            itoa(tagfb->framebuffer_red_field_position, rfp, 10);
            serial_str(rfp);
            serial_char('\n');

            serial_str("green mask size: ");
            char gmsize[15];
            itoa(tagfb->framebuffer_green_mask_size, gmsize, 10);
            serial_str(gmsize);
            serial_char('\n');

            serial_str("green field pos: ");
            char gfp[15];
            itoa(tagfb->framebuffer_green_field_position, gfp, 10);
            serial_str(gfp);
            serial_char('\n');

            serial_str("blue mask size: ");
            char bmsize[15];
            itoa(tagfb->framebuffer_blue_mask_size, bmsize, 10);
            serial_str(bmsize);
            serial_char('\n');

            serial_str("blue field pos: ");
            char bfp[15];
            itoa(tagfb->framebuffer_blue_field_position, bfp, 10);
            serial_str(bfp);
            serial_char('\n');

            serial_char('\n');
    #endif
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