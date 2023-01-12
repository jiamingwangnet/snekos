#include "../console.h"
#include "../graphics.h"
#include "../stdlib.h"

uint32_t foreground = 0xffffff;
uint32_t background = 0x000000;

uint32_t x = 0;
uint32_t y = 0;

uint32_t row = 0;
uint32_t col = 0;

const uint32_t line_pad = 5;
const uint32_t col_pad = 1;

const char* cmdtxt = "| KERNEL CMD :> ";
bool input_mode = false;

char input_buffer[256];
char *buffer_ptr = input_buffer;

void printcmd()
{
    cprintstr(cmdtxt);
    input_mode = true;
}

void init_console(uint32_t sx, uint32_t sy, uint32_t fg, uint32_t bg)
{
    x = sx;
    y = sy;
    foreground = fg;
    background = bg;
    printcmd();
}

void append_buffer(char c)
{
    *buffer_ptr = c;
    buffer_ptr++;
}

void clear_buffer()
{
    for(size_t i = 0; i < 256; i++)
    {
        input_buffer[i] = 0;
    }
    buffer_ptr = input_buffer;
}

void decrease_buffer()
{
    *buffer_ptr = 0;
    if(buffer_ptr > input_buffer)
    {
        buffer_ptr--;
    }
}

void console_keyboard(Key_Info info)
{
    if(!input_mode) return;

    PSF1_font* font = get_font();

    if(!info.release && !info.modifier)
    {
        cprintch(info.key);
        append_buffer(info.key);
    }
    else if(!info.release && info.modifier && info.key == ENTER)
    {
        // clear cursor
        draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background);

        input_mode = false;
        row ++;
        col = 0;

        {
            const char *hello_cmd = "hello";
            char *it = input_buffer;
            char *cmd_it = hello_cmd;

            bool match = true;

            while(*it && *cmd_it)
            {
                match &= (*it == *cmd_it);
                it++;
                cmd_it++;
            }
            match &= (*it == *cmd_it);

            if(match)
            {
                cprintstr("Hello There!");
                input_mode = false;
                row ++;
                col = 0;
            }
        }

        clear_buffer();

        printcmd();
    }
    else if(!info.release && info.modifier && info.key == BACKSPACE)
    {
        if(col == 0 || buffer_ptr == input_buffer) return;
        // clear cursor
        draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background);

        decrease_buffer();
        col --;
        draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background);
    }
}

void cprintch(char c)
{
    PSF1_font* font = get_font();
    draw_char(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, foreground, background, c);
    col++;
}

void cprintstr(const char *str)
{
    while(*str)
        cprintch(*str++);
}

void draw_cursor()
{
    PSF1_font* font = get_font();
    draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y + 10, PSF1_WIDTH, 5, foreground);
}

void console_loop()
{
    if(input_mode) draw_cursor();
}