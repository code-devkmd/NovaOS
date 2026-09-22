#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#define GFX_BLACK 0x00000000U
#define GFX_WHITE 0x00FFFFFFU
#define GFX_BLUE  0x003B82F6U
#define GFX_GREEN 0x0022C55EU
#define GFX_CYAN  0x0006B6D4U
#define GFX_RED   0x00EF4444U
#define GFX_GRAY  0x001F2937U
#define GFX_LIGHT_GRAY 0x009CA3AFU

int graphics_init(uint32_t multiboot_info_address);
int graphics_available(void);
uint32_t graphics_width(void);
uint32_t graphics_height(void);
void graphics_clear(uint32_t color);
void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void graphics_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void graphics_text(uint32_t x, uint32_t y, const char *text, uint32_t fg, uint32_t bg);

#endif
