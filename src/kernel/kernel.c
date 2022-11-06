#include "include/types.h"
#include "include/multiboot2.h"
#include "include/print.h"
#include "include/graphics.h"
#include "include/serial.h"
#include "include/stdlib.h"

extern unsigned long multiboot_info;

void kernel_main()
{
    // for (size_t x = 0; x < 1280; x++)
    // {
    //     for (size_t y = 0; y < 50; y++)
    //     {
    //         put_pixel(x, y, (Color){255, 255, 255});
    //     }
    // }

    if (multiboot_info & 7)
    {
        for (unsigned int i = 0xA0000; i < 0xAFFFF; i++)
        {
            *(uint32_t *)i = 255;
        }
    }

    unsigned long addr = multiboot_info & 0xffffffff;

    char adr[15];
    itoa(addr, adr, 16);
    serial_str("location: 0x");
    serial_str(adr);
    serial_char('\n');

    unsigned size = *(unsigned *)addr;
    struct multiboot_tag *tag;

    char str[15];
    itoa(size, str, 10);
    serial_str(str);
    serial_char('\n');
    serial_char('\n');

    for (tag = (struct multiboot_tag *)(addr + 8); // loop though every tag idk how it works
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        char tagtype[15];
        itoa(tag->type, tagtype, 10);

        char tagsize[15];
        itoa(tag->size, tagsize, 10);

        serial_str("tag: ");
        serial_str(tagtype);
        serial_str(" size: ");
        serial_str(tagsize);
        serial_char('\n');

        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            serial_char('\n');
            multiboot_uint32_t color;
            unsigned i;
            struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
            volatile void *fb = (void *)(unsigned long long)tagfb->common.framebuffer_addr; // cannot dereference because it is not mapped


#pragma region 
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

            serial_str("fb type: ");
            char fbtype[15];
            itoa(tagfb->common.framebuffer_type, fbtype, 10);
            serial_str(fbtype);
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
#pragma endregion
            color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
                    << tagfb->framebuffer_blue_field_position;

            multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * 0 + 4 * 0;
            uint64_t value = 0;

            serial_str("fb pixel[0]: 0x");

            char fbpixel[15];
            itoa(value, fbpixel, 16);
            char fbpixeldec[15];
            itoa(value, fbpixeldec, 10);

            serial_str(fbpixel);
            serial_str(" (");
            serial_str(fbpixeldec);
            serial_str(")");
            
            serial_char('\n');
            serial_char('\n');
            
            // for (i = 0; i < tagfb->common.framebuffer_width && i < tagfb->common.framebuffer_height; i++)
            // {
            //     multiboot_uint32_t *pixel = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
            //     *pixel = color;
            // }
        }
    }
}