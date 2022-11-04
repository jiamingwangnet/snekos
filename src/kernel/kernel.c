#include "headers/types.h"
#include "headers/multiboot2.h"
#include "headers/print.h"
#include "headers/graphics.h"

void kernel_main()
{
    // for (size_t x = 0; x < 1280; x++)
    // {
    //     for (size_t y = 0; y < 50; y++)
    //     {
    //         put_pixel(x, y, (Color){255, 255, 255});
    //     }
    // }

    for(unsigned int i = 0xA0000; i < 0xAFFFF; i++)
    {
        *(uint32_t*)i = 255;
    }
}