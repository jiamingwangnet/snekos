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

    
    if(!(multiboot_info & 7))
    {
        for(unsigned int i = 0xA0000; i < 0xAFFFF; i++)
        {
            *(uint32_t*)i = 255;
        }
    }

    unsigned long addr = multiboot_info & 0xffffffff;

    unsigned size = *(unsigned *) addr;
    struct multiboot_tag *tag;

    char str[15];
    itoa(size, str, 10);
    serial_str(str);
    serial_char('\n');
    serial_char('\n');

    for (tag = (struct multiboot_tag *) (addr + 8); // loop though every tag idk how it works
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag 
                                       + ((tag->size + 7) & ~7)))
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
    }
}