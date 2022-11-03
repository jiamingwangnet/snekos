#include "graphics.h"
#include "types.h"

void put_pixel(int x, int y, uint8_t color)
{
    uint8_t* location = (uint8_t*)0xA0000 + 320 * y + x;
    *location = color;
}