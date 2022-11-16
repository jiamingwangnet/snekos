#pragma once
#include "types.h"

#define COM1 0x3F8

void out(uint16_t port, uint8_t data);
void in(uint16_t port);