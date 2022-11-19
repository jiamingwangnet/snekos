#include "../io.h"

void out(uint16_t port, uint8_t data)
{
    __asm__("out %%dx, %%ax" :: "d"(port), "a"(data));
}

uint8_t in(uint16_t port)
{
    uint8_t o;
    __asm__("in %%ax, %%dx" : "=a"(o) : "d"(port));
    return o;
}