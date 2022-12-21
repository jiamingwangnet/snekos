#include "../console.h"
#include "../graphics.h"

Color foreground = {255,255,255};
Color background = {0,0,0};
framebuffer_tag* framebuffer = NULL;

uint32_t x = 0;
uint32_t y = 0;

uint32_t row = 0;
uint32_t col = 0;

const uint32_t line_pad = 5;
const uint32_t col_pad = 1;

const char* cmdtxt = "| KERNEL CMD :> ";
bool input_mode = false;

void printcmd()
{
    cprintstr(cmdtxt);
    input_mode = true;
}

void init_console(uint32_t sx, uint32_t sy, Color fg, Color bg, framebuffer_tag* fb)
{
    x = sx;
    y = sy;
    framebuffer = fb;
    foreground = fg;
    background = bg;
}

void console_keyboard(Key_Info info)
{
    if(!input_mode) return;

    PSF1_font* font = get_font();

    if(!info.release && !info.modifier)
    {
        cprintch(info.key);
    }
    else if(!info.release && info.modifier && info.key == ENTER)
    {
        // clear cursor
        draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background, framebuffer);

        input_mode = false;
        row ++;
        col = 0;

        printcmd();
    }
    else if(!info.release && info.modifier && info.key == BACKSPACE)
    {
        if(col == 0) return;
        // clear cursor
        draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background, framebuffer);

        col --;
        draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, PSF1_WIDTH + col_pad, font->charsize + line_pad, background, framebuffer);
    }
}

void cprintch(char c)
{
    PSF1_font* font = get_font();
    draw_char(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y, foreground, background, c, framebuffer);
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
    draw_rect(col * (PSF1_WIDTH + col_pad) + x, row * (font->charsize + line_pad) + y + 10, PSF1_WIDTH, 5, foreground, framebuffer);
}

void console_loop()
{
    if(input_mode) draw_cursor();
}