#pragma once

#include <stdbool.h>
#include "../stdlib/types.h"

#define KEY_MAX 0x3A // skipping the Function keys

#define ESCAPE 0x01
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define L_SHIFT 0x2A
#define R_SHIFT 0x36
#define L_SHIFT_RELEASE 0xAA
#define R_SHIFT_RELEASE 0xB6
#define ARROW_UP   0x48
#define ARROW_DOWN 0x50

#define RELEASE_OFFSET 0x80

typedef struct
{
    char key;
    uint8_t raw;
    bool modifier;
    bool release;
} Key_Info;

typedef void (*key_callback_t)(Key_Info);

void handle_key();
void init_keyboard();

// the last attched callback will be executed first
void attach_keyboard(key_callback_t callback);

// test handler
void serial_keyboard(Key_Info);