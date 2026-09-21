#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

#define VGA_COLOR_BLACK        0x0
#define VGA_COLOR_BLUE         0x1
#define VGA_COLOR_GREEN        0x2
#define VGA_COLOR_CYAN         0x3
#define VGA_COLOR_RED          0x4
#define VGA_COLOR_MAGENTA      0x5
#define VGA_COLOR_BROWN       0x6
#define VGA_COLOR_LIGHT_GREY  0x7
#define VGA_COLOR_DARK_GREY    0x8
#define VGA_COLOR_LIGHT_BLUE   0x9
#define VGA_COLOR_LIGHT_GREEN  0xA
#define VGA_COLOR_LIGHT_CYAN   0xB
#define VGA_COLOR_LIGHT_RED    0xC
#define VGA_COLOR_LIGHT_MAGENTA 0xD
#define VGA_COLOR_LIGHT_BROWN  0xE
#define VGA_COLOR_WHITE        0xF

#define VGA_ENTRY_COLOR(fg, bg) ((uint8_t)(fg) | ((uint8_t)(bg) << 4))

void terminal_init(void);
void terminal_clear(void);
void terminal_putchar(char c);
void terminal_write(const char *str);
void terminal_setcolor(uint8_t color);
void terminal_get_cursor(size_t *row, size_t *column);
void terminal_set_cursor(size_t row, size_t column);
void terminal_cursor_left(void);
void terminal_cursor_right(void);
void terminal_cursor_home(void);

#endif
