#include "../include/io/io.h"

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

inline __attribute__((always_inline)) void outl(uint16_t port, uint32_t data)
{
    __asm__ volatile("out %1, %0" : : "a"(data), "Nd"(port));
}

inline __attribute__((always_inline)) uint32_t inl(uint16_t port)
{
    uint32_t o;
    __asm__ volatile("in %0, %1" : "=a"(o) : "Nd"(port));
    return o;
}

inline __attribute__((always_inline)) void outsl(uint16_t port, uint32_t *buffer, uint32_t count)
{
    while(--count) outl(port, *buffer++);
}

inline __attribute__((always_inline)) void insl(uint16_t port, uint32_t *buffer, uint32_t count)
{
    while(--count) *buffer ++ = inl(port);
}

inline __attribute__((always_inline)) void outw(uint16_t port, uint16_t data)
{
    __asm__ volatile("out %1, %0" : : "a"(data), "Nd"(port));
}

inline __attribute__((always_inline)) uint16_t inw(uint16_t port)
{
    uint16_t o;
    __asm__ volatile("in %0, %1" : "=a"(o) : "Nd"(port));
    return o;
}

inline __attribute__((always_inline)) void outsw(uint16_t port, uint16_t *buffer, uint32_t count)
{
    while(--count) outw(port, *buffer++);
}

inline __attribute__((always_inline)) void insw(uint16_t port, uint16_t *buffer, uint32_t count)
{
    while(--count) *buffer ++ = inw(port);
}