#pragma once
#include "../stdlib/types.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8
#define COM5 0x5F8
#define COM6 0x4F8
#define COM7 0x5E8
#define COM8 0x4E8

void out(uint16_t port, uint8_t data);
uint8_t in(uint16_t port);

void outl(uint16_t port, uint32_t data);
uint32_t inl(uint16_t port);
// read into a buffer
void insl(uint16_t port, uint32_t *buffer, uint32_t count);
void outsl(uint16_t port, uint32_t *buffer, uint32_t count);

void outw(uint16_t port, uint16_t data);
uint16_t inw(uint16_t port);

void insw(uint16_t port, uint16_t *buffer, uint32_t count);
void outsw(uint16_t port, uint16_t *buffer, uint32_t count);