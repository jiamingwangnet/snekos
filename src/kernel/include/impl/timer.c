#include "../timer.h"
#include "../io.h"

volatile uint64_t time = 0;

#define I86_PIT_REG_COUNTER0 0x40
#define I86_PIT_REG_COUNTER1 0x41
#define I86_PIT_REG_COUNTER2 0x42
#define I86_PIT_REG_COMMAND  0x43

void init_timer()
{
    add_handler(IRQ0, inc_time);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    uint16_t divisor = (uint16_t) (1193180 / 1500);

    // Send the command byte.
    out(I86_PIT_REG_COMMAND, 0x36);

    // Send the frequency divisor.
    out(I86_PIT_REG_COUNTER0, (uint8_t) (divisor & 0xFF));
    out(I86_PIT_REG_COUNTER0, (uint8_t) (divisor >> 8 & 0xFF));

    time = 0;
}

void inc_time()
{
    time ++;
}

uint64_t get_time() 
{
    return time;
}

void wait_ticks(uint64_t w_time)
{
    volatile uint64_t stop = time + w_time;
    while(time < stop)
    {
        __asm__ volatile("nop");
    }
    // DO NOT REMOVE!!!
    get_time(); // why do i need this here?????? breaks if removed
}