#pragma once

#include "../stdlib/types.h"

#define PSF_FONT_MAGIC 0x0436
#define PSF1_WIDTH 8
 
// using psf1 because i cant find psf2 fonts
typedef struct {
    uint16_t magic;     /* Magic number */
    uint8_t mode;         /* PSF font mode */
    uint8_t charsize;     /* Character size */
} PSF1_font;

extern char _binary_assets_vga_font_psf_start;
extern char _binary_assets_vga_font_psf_end;

void init_font();
void draw_char(uint32_t x, uint32_t y, uint32_t foreground, uint32_t background, char c);
void draw_str(uint32_t x, uint32_t y, uint32_t foreground, uint32_t background, const char* str);
PSF1_font* get_font();