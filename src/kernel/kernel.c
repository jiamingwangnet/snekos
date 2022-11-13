#include "include/types.h"
#include "include/multiboot2.h"
#include "include/print.h"
#include "include/graphics.h"
#include "include/serial.h"
#include "include/stdlib.h"

extern unsigned long multiboot_info;
extern unsigned long page_table_l2;
extern unsigned long framebuffer;
extern void map_framebuffer(void);

void kernel_main()
{
    if (multiboot_info & 7)
    {
        for (unsigned int i = 0xA0000; i < 0xAFFFF; i+=4)
        {
            *(uint32_t *)(i + 2) = 255;
        }
    }

    unsigned long addr = multiboot_info & 0xffffffff;

    char pagel2[15];
    itoa((unsigned long)page_table_l2, pagel2, 16);
    serial_str("page l2: 0x");
    serial_str(pagel2);
    serial_char('\n');

    // unsigned long mapped = 0xfd000000 | 0b10000011;
    // *(unsigned long *)(page_table_l2 + 8) = mapped;

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
            struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;

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

            unsigned pitch = tagfb->common.framebuffer_pitch;
            unsigned bpp = tagfb->common.framebuffer_bpp;

            // map framebuffer
            framebuffer = tagfb->common.framebuffer_addr;
            map_framebuffer(); // framebuffer mapped to 0x00000

            for(unsigned i = 0; i < 1280 * 205; i++) // cant go over 205 lines idk
                *(uint32_t*)(i * (bpp / 8)) = 0xffffffff;

            // overwrites kernel code causing crash
            // map the kerenel to a higher address might be a solution
            // *(uint32_t*)(206 * pitch) = 0xffffffff;
            
            serial_char('\n');
        }
    }
}