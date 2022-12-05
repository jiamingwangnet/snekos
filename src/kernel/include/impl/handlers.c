#include "../handlers.h"
#include "../keyboard.h"
#include "../io.h"
#include "../serial.h"

handler_t handlers[256] = {};

void irq_handler(uint8_t id)
{
    serial_str("\n\nIrq Handled ");
    char c_id[15];
    itoa(id, c_id, 10);
    serial_str(c_id);
    serial_char('\n');

    handler_t handler = handlers[id];
    if(handler) handler();

    if(id >= 40) // IRQ 8
        out(PIC2_COMMAND, PIC_EOI);

    out(PIC1_COMMAND, PIC_EOI);
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