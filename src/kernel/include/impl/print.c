#include "../print.h"
#include "../types.h"

struct Char
{
    uint8_t character;
    uint8_t color;
};

struct Char* buffer = (struct Char*) 0xb8000; // create a buffer at video memory address (0xb8000)

size_t col = 0;
size_t row = 0;
// foreground | background << 4
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;


void print_char(char c)
{
    if(c == '\n')
    {
        print_newline();
        return;
    }

    if(col > NUM_COLS)
    {
        print_newline();
    }

    buffer[row * NUM_COLS + col] = (struct Char) {
        .character = c,
        .color = color,
    };

    col++;
}

void print_newline()
{
    col = 0;

    if(row < NUM_ROWS - 1)
    {
        row++;
        return;
    }

    for(size_t row = 1; row < NUM_ROWS; row++)
    {
        for(size_t col = 0; col < NUM_COLS; col++)
        {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }

    clear_row(NUM_ROWS - 1);
}

void print_str(char* str)
{
    for(; *str != 0; str++)
    {
        print_char(*str);
    }
}

void clear_row(size_t row)
{
    for(size_t i = 0; i < NUM_COLS; i++)
    {
        buffer[row * NUM_COLS + i] = (struct Char) {
            .character = ' ',
            .color = color,
        };
    }
}

void clear_srn()
{
    for(size_t row = 0; row < NUM_ROWS; row++)
    {
        clear_row(row);
    }
}