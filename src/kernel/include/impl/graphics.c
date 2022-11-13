#include "../graphics.h"
#include "../types.h"

const static size_t width = 1280;
const static size_t height = 800;

void put_pixel(int x, int y, Color color)
{
    uint8_t* location = (uint8_t*)(0xA0000 + (4 * width * y) + (4 * x));
    *(location + 0) = color.b;
    *(location + 1) = color.g;
    *(location + 2) = color.r;
}