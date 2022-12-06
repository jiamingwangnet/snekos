#include "../keyboard.h"
#include "../io.h"
#include "../serial.h"
#include "../handlers.h"

// US QWERTY
const char keys_lower[KEY_MAX] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0,
        0, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
            'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
              'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

const char keys_upper[KEY_MAX] = {

};

void init_keyboard(key_callback_t callback)
{
    add_handler(IRQ1, &handle_key);
    key_callback = callback;
}

void handle_key()
{
    char c = in(0x60);

    /*
        TODO:
        - add function keys
    */

    char key = c > KEY_MAX ? keys_lower[c - RELEASE_OFFSET] : keys_lower[c];
    bool release = c > KEY_MAX;
    bool modifier = key == 0;
    if(modifier)
        key = c;

    Key_Info info = {
        .key = key,
        .release = release, // skip the function keys and assume they're release keys
        .modifier = modifier,
    };
    key_callback(info);
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