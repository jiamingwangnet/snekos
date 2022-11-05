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
    unsigned size = *(unsigned *) (multiboot_info & 0xffffffff);
    char str[15];
    itoa(size, str, 10);
    serial_str(str);
}