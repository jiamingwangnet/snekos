#include "../keyboard.h"
#include "../io.h"
#include "../serial.h"

void handle_key()
{
    char c = in(0x60);
    serial_char('c');
}