#pragma once

#include "../stdlib/types.h"
#include "../interrupts/idt.h"

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef void (*handler_t)(void);

void irq_handler(reg_status_t status);
void add_handler(uint8_t index, handler_t hdlr);