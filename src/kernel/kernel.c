#include "include/types.h"
#include "include/serial.h"
#include "include/stdlib.h"
#include "include/multiboot_init.h"
#include "include/graphics.h"
#include "include/idt.h"
#include "include/keyboard.h"
#include "include/timer.h"
#include "include/font.h"
#include "include/console.h"
#include "include/kmalloc.h"

void kernel_main()
{
    init_multiboot();

    init_serial();
    init_idt();
    init_font();

    init_timer();
    init_heap();

    init_graphics();

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

    draw_rect(0,0,SCRN_WIDTH,SCRN_HEIGHT,0xffffff);

    // draw smile

    // mouth
    draw_rect(SCRN_WIDTH/2 - 250, SCRN_HEIGHT/2 + 110, 500, 50, 0);
    draw_rect(SCRN_WIDTH/2 - 250, SCRN_HEIGHT/2 + 10, 50, 100, 0);
    draw_rect(SCRN_WIDTH/2 + 200, SCRN_HEIGHT/2 + 10, 50, 100, 0);

    // eyes
    draw_rect(SCRN_WIDTH/2 - 130, 200, 60, 120,0);
    draw_rect(SCRN_WIDTH/2 + 70, 200, 60, 120, 0);

    update_buffer();

    wait_ticks(3000);

    draw_rect(0, 0, SCRN_WIDTH, SCRN_HEIGHT, 0x0f0f0f);
    draw_str(20, 20, 0xe0ffeb,0xf0f0f,
                    "Hello, World!\n"
                     "TEST CMD!!!\n"
                     "The Quick Brown Fox Jumps Over The Lazy Dog\n"
                     "the quick brown fox fox jumps over the lazy dog.\n"
                     "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG.\n"
                     "1234567890\n"
                     "~!@#$%^&*()_+<>[]{}");
    
    update_buffer();

    wait_ticks(1500);
    
    draw_rect(0, 0, SCRN_WIDTH, SCRN_HEIGHT, 0x0f0f0f);
    init_console(20, 20, 0xe0ffeb, 0x0f0f0f);
    init_keyboard(console_keyboard);

    update_buffer();

    bool right = true;
    for(uint32_t x = 0;;)
    {
        // clear path
        draw_rect(0,0,SCRN_WIDTH, 10, 0x303030);
        draw_rect(x, 0, 10, 10, 0x28c77a);
        if(right)
            x++;
        else
            x--;
        if(x + 10 >= SCRN_WIDTH)
            right = false;
        else if(x <= 0)
            right = true;

        console_loop();

        if(get_time() % (1000/60) == 0)
        {
            update_buffer();
        }
    }
}