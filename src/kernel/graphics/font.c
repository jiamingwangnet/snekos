#include "../include/graphics/font.h"
#include "../include/io/serial.h"
#include "../include/stdlib/stdlib.h"
#include "../include/graphics/graphics.h"

PSF1_font* font = NULL;

void init_font() 
{
    font = (PSF1_font*)&_binary_assets_vga_font_psf_start;
    if(font->magic != PSF_FONT_MAGIC)
    {
        font = NULL;
    }
#ifdef DEBUG_LOG
    char magic[16];
    char mode[16];
    char charsize[16];
    itoa(font->magic, magic, 16);
    itoa(font->mode, mode, 10);
    itoa(font->charsize, charsize, 10);

    serial_str(magic);
    serial_char('\n');
    serial_str(mode);
    serial_char('\n');
    serial_str(charsize);
    serial_char('\n');
#endif
}

void draw_char(uint32_t x, uint32_t y, uint32_t foreground, uint32_t background, char c)
{
    if(font == NULL || c == '\n') return;

    uint8_t* glyph = (uint8_t*)(&_binary_assets_vga_font_psf_start + sizeof(PSF1_font) + c * (uint32_t)font->charsize);

    for(uint32_t dy = y; dy < (uint32_t)font->charsize + y; dy++)
    {
        uint8_t row = glyph[dy - y];
        for(uint32_t dx = x; dx < PSF1_WIDTH + x; dx++)
        {
            uint8_t mask = 0b10000000 >> (dx - x);
            uint32_t color = (row & mask) == 0 ? background : foreground;
            put_pixel(dx, dy, color);
        }
    }
}

void draw_str(uint32_t x, uint32_t y, uint32_t foreground, uint32_t background, const char* str)
{
    if(font == NULL) return;
    uint32_t width = PSF1_WIDTH;
    uint32_t line = y;

    for(uint32_t dx = x; *str != 0; str++, dx += width + 1)
    {
        if(*(str - 1) == '\n')
        {
            line += font->charsize + 5;
            dx = x;
        }

        draw_char(dx, line, foreground, background, *str);
    }
}

PSF1_font* get_font() { return font; }