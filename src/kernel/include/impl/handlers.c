#include "../handlers.h"
#include "../keyboard.h"

handler_t handlers[256] = {};

void irq_handler(uint8_t id)
{
    handler_t handler = handlers[id];
    if(handler) handler();
}

void add_handler(uint8_t index, handler_t hdlr)
{
    handlers[index] = hdlr;
}

// handlers
void keyboard_handler()
{
    handle_key();
}