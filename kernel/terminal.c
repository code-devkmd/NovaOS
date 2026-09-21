#include "terminal.h"

#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_CURSOR_HIGH 0x0E
#define VGA_CURSOR_LOW  0x0F

static volatile uint16_t *const VGA_MEMORY = (volatile uint16_t *)0xB8000;
static size_t row = 0;
static size_t column = 0;
static uint8_t color = VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint16_t vga_entry(char character, uint8_t entry_color)
{
    return (uint16_t)character | ((uint16_t)entry_color << 8);
}

static void update_hardware_cursor(void)
{
    uint16_t position = (uint16_t)(row * VGA_WIDTH + column);
    outb(VGA_CRTC_INDEX, VGA_CURSOR_HIGH);
    outb(VGA_CRTC_DATA, (uint8_t)(position >> 8));
    outb(VGA_CRTC_INDEX, VGA_CURSOR_LOW);
    outb(VGA_CRTC_DATA, (uint8_t)(position & 0xFF));
}

static void terminal_scroll(void)
{
    for (size_t y = 1; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];

    for (size_t x = 0; x < VGA_WIDTH; x++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', color);

    row = VGA_HEIGHT - 1;
}

static void terminal_newline(void)
{
    column = 0;
    row++;
    if (row >= VGA_HEIGHT) terminal_scroll();
}

static void terminal_backspace(void)
{
    if (column > 0) column--;
    else
    {
        if (row == 0) return;
        row--;
        column = VGA_WIDTH - 1;
    }
    VGA_MEMORY[row * VGA_WIDTH + column] = vga_entry(' ', color);
}

void terminal_init(void)
{
    terminal_clear();
}

void terminal_setcolor(uint8_t new_color) { color = new_color; }

void terminal_clear(void)
{
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', color);
    row = 0;
    column = 0;
    update_hardware_cursor();
}

void terminal_putchar(char c)
{
    if (c == '\n') { terminal_newline(); update_hardware_cursor(); return; }
    if (c == '\r') { column = 0; update_hardware_cursor(); return; }
    if (c == '\b') { terminal_backspace(); update_hardware_cursor(); return; }

    VGA_MEMORY[row * VGA_WIDTH + column] = vga_entry(c, color);
    column++;
    if (column >= VGA_WIDTH)
    {
        column = 0;
        row++;
        if (row >= VGA_HEIGHT) terminal_scroll();
    }
    update_hardware_cursor();
}

void terminal_write(const char *str)
{
    if (!str) return;
    while (*str) terminal_putchar(*str++);
}

void terminal_get_cursor(size_t *out_row, size_t *out_column)
{
    if (out_row) *out_row = row;
    if (out_column) *out_column = column;
}

void terminal_set_cursor(size_t new_row, size_t new_column)
{
    if (new_row >= VGA_HEIGHT) new_row = VGA_HEIGHT - 1;
    if (new_column >= VGA_WIDTH) new_column = VGA_WIDTH - 1;
    row = new_row;
    column = new_column;
    update_hardware_cursor();
}

void terminal_cursor_left(void)
{
    if (column > 0) column--;
    else if (row > 0) { row--; column = VGA_WIDTH - 1; }
    update_hardware_cursor();
}

void terminal_cursor_right(void)
{
    if (column + 1 < VGA_WIDTH) column++;
    else if (row + 1 < VGA_HEIGHT) { row++; column = 0; }
    update_hardware_cursor();
}

void terminal_cursor_home(void)
{
    column = 0;
    update_hardware_cursor();
}
