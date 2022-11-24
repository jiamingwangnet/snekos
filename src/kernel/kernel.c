#include "include/types.h"
#include "include/serial.h"
#include "include/stdlib.h"
#include "include/multiboot_init.h"
#include "include/graphics.h"
#include "include/idt.h"
#include "include/handlers.h"

void kernel_main()
{
    init_framebuffer();

    init_serial();
    init_pic();
    init_idt();

    add_handler(33, &keyboard_handler);

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

    for(uint32_t y = 0; y < SCRN_HEIGHT; y++)
    {
        for(uint32_t x = 0; x < SCRN_WIDTH; x++)
        {
            put_pixel(x, y, (Color){255,255,255}, tagfb);
        }
    }

    // draw smile

    // mouth
    draw_rect(SCRN_WIDTH/2 - 250, SCRN_HEIGHT/2 + 110, 500, 50, (Color){0,0,0}, tagfb);
    draw_rect(SCRN_WIDTH/2 - 250, SCRN_HEIGHT/2 + 10, 50, 100, (Color){0,0,0}, tagfb);
    draw_rect(SCRN_WIDTH/2 + 200, SCRN_HEIGHT/2 + 10, 50, 100, (Color){0,0,0}, tagfb);

    // eyes
    draw_rect(SCRN_WIDTH/2 - 130, 200, 60, 120, (Color){0,0,0}, tagfb);
    draw_rect(SCRN_WIDTH/2 + 70, 200, 60, 120, (Color){0,0,0}, tagfb);

    __asm__("int 33");
    
    bool right = true;
    for(int x = 0;;)
    {
        // clear path
        draw_rect(0,0,SCRN_WIDTH, 100, (Color){255,255,255}, tagfb);

        draw_rect(x, 0, 100, 100, (Color){122, 199, 40}, tagfb);
        if(right)
            x++;
        else
            x--;

        if(x + 100 >= SCRN_WIDTH)
            right = false;
        else if(x <= 0)
            right = true;
    }
}