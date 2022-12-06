#pragma once

#include "types.h"
#include "idt.h"

typedef void (*handler_t)(void);

void irq_handler(uint8_t id);
void add_handler(uint8_t index, handler_t hdlr);