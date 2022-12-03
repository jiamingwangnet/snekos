#include "../io.h"

inline __attribute__((always_inline)) void out(uint16_t port, uint8_t data)
{
    __asm__ volatile("out %1, %0" : : "a"(data), "Nd"(port));
}

inline __attribute__((always_inline)) uint8_t in(uint16_t port)
{
    uint8_t o;
    __asm__ volatile("in %0, %1" : "=a"(o) : "Nd"(port));
    return o;
}