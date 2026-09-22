#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

/* Each glyph in font8x8 is an 8x8 bitmap. FONT_SCALE draws every glyph
 * pixel as an NxN block so text is legible at high resolutions like
 * 1024x768 instead of rendering as tiny 8px characters. */
#define FONT_SCALE 2

int framebuffer_init(uint32_t multiboot_info_address);
int framebuffer_available(void);
uint32_t framebuffer_width(void);
uint32_t framebuffer_height(void);
uint32_t framebuffer_pitch(void);
uint8_t framebuffer_bpp(void);

void framebuffer_clear(uint32_t color);
void framebuffer_putpixel(uint32_t x, uint32_t y, uint32_t color);
void framebuffer_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void framebuffer_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void framebuffer_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg);
void framebuffer_draw_text(uint32_t x, uint32_t y, const char *text, uint32_t fg, uint32_t bg);

#endif