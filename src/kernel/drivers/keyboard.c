#include "../include/drivers/keyboard.h"
#include "../include/io/io.h"
#include "../include/io/serial.h"
#include "../include/interrupts/handlers.h"
#include "../include/console/console.h"

// US QWERTY
const char keys_lower[KEY_MAX] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
        0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
            'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
              'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

const char keys_upper[KEY_MAX] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
        0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, 0,
            'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
              'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' '
};

bool holding_lshift = false;
bool holding_rshift = false;

key_callback_t callbacks[256];
key_callback_t *callback_end = callbacks;

void init_keyboard()
{
    add_handler(IRQ1, &handle_key);
    kprintf("Initialised keyboard.\n");
}

void attach_keyboard(key_callback_t callback)
{
    *callback_end = callback;
    callback_end++;
}

void handle_callbacks(Key_Info info)
{
    key_callback_t *tmp = callback_end;
    while(tmp != callbacks)
        (*--tmp)(info);
}

void handle_key()
{
    uint8_t c = in(0x60);

    /*
        TODO:
        - add function keys
    */

    if(c == L_SHIFT)
    {
        holding_lshift = true;
    }
    else if(c == L_SHIFT_RELEASE)
    {
        holding_lshift = false;
    }

    if(c == R_SHIFT)
    {
        holding_rshift = true;
    }
    else if(c == R_SHIFT_RELEASE)
    {
        holding_rshift = false;
    }

    const char* keys = holding_lshift | holding_rshift ? keys_upper : keys_lower;

    char key = c > KEY_MAX ? keys[c - RELEASE_OFFSET] : keys[c];
    bool release = c & 0x80;
    bool modifier = key == 0;
    if(modifier)
        key = c;

    Key_Info info = {
        .key = key,
        .raw = c,
        .release = release, // skip the function keys and assume they're release keys
        .modifier = modifier,
    };
    handle_callbacks(info);
}

void serial_keyboard(Key_Info info)
{
    if(!info.release && !info.modifier)
    {
        serial_char(info.key);
    }
    else if(!info.release && info.modifier && info.key == ENTER)
    {
        serial_char('\n');
    }
}