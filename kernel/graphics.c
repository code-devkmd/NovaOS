#include "graphics.h"
#include "framebuffer.h"

int graphics_init(uint32_t multiboot_info_address)
{
    return framebuffer_init(multiboot_info_address);
}

int graphics_available(void) { return framebuffer_available(); }
uint32_t graphics_width(void) { return framebuffer_width(); }
uint32_t graphics_height(void) { return framebuffer_height(); }
void graphics_clear(uint32_t color) { framebuffer_clear(color); }
void graphics_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{ framebuffer_fill_rect(x, y, width, height, color); }
void graphics_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{ framebuffer_draw_rect(x, y, width, height, color); }
void graphics_text(uint32_t x, uint32_t y, const char *text, uint32_t fg, uint32_t bg)
{ framebuffer_draw_text(x, y, text, fg, bg); }
