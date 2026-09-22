#include "terminal.h"
#include "framebuffer.h"
#include "mouse.h"
#include <stdint.h>

#define MAX_TERMINAL_COLUMNS 128
#define MAX_TERMINAL_ROWS 96
#define FONT_WIDTH  (8U * FONT_SCALE)
#define FONT_HEIGHT (8U * FONT_SCALE)

#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA  0x3D5
#define VGA_CURSOR_HIGH 0x0E
#define VGA_CURSOR_LOW  0x0F

static volatile uint16_t *const VGA_MEMORY = (volatile uint16_t *)0xB8000;
static size_t row = 0;
static size_t column = 0;
static size_t columns = VGA_WIDTH;
static size_t rows = VGA_HEIGHT;
static uint8_t color = VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
static char text_buffer[MAX_TERMINAL_ROWS][MAX_TERMINAL_COLUMNS];
static uint8_t color_buffer[MAX_TERMINAL_ROWS][MAX_TERMINAL_COLUMNS];
static int use_framebuffer;
static int cursor_visible = 1;
static uint32_t cursor_ticks;

static const uint32_t vga_rgb[16] =
{
    0x00000000U, 0x000000AAU, 0x0000AA00U, 0x0000AAAAU,
    0x00AA0000U, 0x00AA00AAU, 0x00AA5500U, 0x00AAAAAAU,
    0x00555555U, 0x005555FFU, 0x0055FF55U, 0x0055FFFFU,
    0x00FF5555U, 0x00FF55FFU, 0x00FFFF55U, 0x00FFFFFFU
};

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

static void framebuffer_draw_cell(size_t y, size_t x)
{
    uint8_t entry_color = color_buffer[y][x];
    uint32_t fg = vga_rgb[entry_color & 0x0F];
    uint32_t bg = vga_rgb[(entry_color >> 4) & 0x0F];
    framebuffer_draw_char((uint32_t)x * FONT_WIDTH, (uint32_t)y * FONT_HEIGHT,
                          text_buffer[y][x], fg, bg);
}

static void framebuffer_draw_cursor(void)
{
    if (!use_framebuffer || !cursor_visible || row >= rows || column >= columns) return;
    framebuffer_fill_rect((uint32_t)column * FONT_WIDTH,
                          (uint32_t)row * FONT_HEIGHT + FONT_HEIGHT - 2,
                          FONT_WIDTH, 2, vga_rgb[color & 0x0F]);
}

static void framebuffer_hide_cursor(void)
{
    if (!use_framebuffer || row >= rows || column >= columns) return;
    framebuffer_draw_cell(row, column);
}

static void framebuffer_redraw_all(void)
{
    size_t y, x;
    if (!use_framebuffer) return;
    for (y = 0; y < rows; y++)
        for (x = 0; x < columns; x++)
            framebuffer_draw_cell(y, x);
    framebuffer_draw_cursor();
}

static void clear_buffer(void)
{
    size_t y, x;
    for (y = 0; y < MAX_TERMINAL_ROWS; y++)
        for (x = 0; x < MAX_TERMINAL_COLUMNS; x++)
        {
            text_buffer[y][x] = ' ';
            color_buffer[y][x] = color;
        }
}

static void terminal_scroll(void)
{
    size_t y, x;
    for (y = 1; y < rows; y++)
        for (x = 0; x < columns; x++)
        {
            text_buffer[y - 1][x] = text_buffer[y][x];
            color_buffer[y - 1][x] = color_buffer[y][x];
        }

    for (x = 0; x < columns; x++)
    {
        text_buffer[rows - 1][x] = ' ';
        color_buffer[rows - 1][x] = color;
    }
    row = rows - 1;
}

/* Returns 1 if the screen actually scrolled (every visible cell moved and
 * needs a full repaint), 0 if the cursor simply advanced to a fresh row
 * that was already blank on screen. Only the scrolled case needs to touch
 * every cell; doing a full-screen redraw on every plain newline is what
 * made the terminal feel like it froze after a single line of output. */
static int terminal_newline(void)
{
    column = 0;
    row++;
    if (row >= rows)
    {
        terminal_scroll();
        return 1;
    }
    return 0;
}

static void terminal_backspace(void)
{
    if (column > 0) column--;
    else
    {
        if (row == 0) return;
        row--;
        column = columns - 1;
    }
    text_buffer[row][column] = ' ';
    color_buffer[row][column] = color;
    if (use_framebuffer) framebuffer_draw_cell(row, column);
    else VGA_MEMORY[row * VGA_WIDTH + column] = vga_entry(' ', color);
}

void terminal_init(void)
{
    use_framebuffer = framebuffer_available();
    if (use_framebuffer)
    {
        columns = framebuffer_width() / FONT_WIDTH;
        rows = framebuffer_height() / FONT_HEIGHT;
        if (columns > MAX_TERMINAL_COLUMNS) columns = MAX_TERMINAL_COLUMNS;
        if (rows > MAX_TERMINAL_ROWS) rows = MAX_TERMINAL_ROWS;
        if (columns == 0) columns = VGA_WIDTH;
        if (rows == 0) rows = VGA_HEIGHT;
    }
    else
    {
        columns = VGA_WIDTH;
        rows = VGA_HEIGHT;
    }
    clear_buffer();
    terminal_clear();
    mouse_cursor_draw();
}

void terminal_setcolor(uint8_t new_color)
{
    if (use_framebuffer) framebuffer_hide_cursor();
    color = new_color;
    if (use_framebuffer) framebuffer_draw_cursor();
}

void terminal_clear(void)
{
    row = 0;
    column = 0;
    cursor_visible = 1;
    cursor_ticks = 0;
    clear_buffer();

    if (use_framebuffer)
    {
        framebuffer_clear(vga_rgb[(color >> 4) & 0x0F]);
        framebuffer_redraw_all();
    }
    else
    {
        size_t y, x;
        for (y = 0; y < VGA_HEIGHT; y++)
            for (x = 0; x < VGA_WIDTH; x++)
                VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', color);
        update_hardware_cursor();
    }
}

void terminal_redraw_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    size_t first_x, first_y, last_x, last_y;
    size_t cx, cy;

    if (!use_framebuffer || width == 0 || height == 0)
        return;

    if (x >= framebuffer_width() || y >= framebuffer_height())
        return;

    if (width > framebuffer_width() - x)
        width = framebuffer_width() - x;
    if (height > framebuffer_height() - y)
        height = framebuffer_height() - y;

    first_x = x / FONT_WIDTH;
    first_y = y / FONT_HEIGHT;
    last_x = (x + width - 1) / FONT_WIDTH;
    last_y = (y + height - 1) / FONT_HEIGHT;

    if (last_x >= columns) last_x = columns - 1;
    if (last_y >= rows) last_y = rows - 1;

    for (cy = first_y; cy <= last_y; cy++)
        for (cx = first_x; cx <= last_x; cx++)
            framebuffer_draw_cell(cy, cx);
}

void terminal_putchar(char c)
{
    int redraw_all = 0;
    mouse_cursor_hide();
    if (use_framebuffer) framebuffer_hide_cursor();

    if (c == '\n')
        redraw_all = terminal_newline();
    else if (c == '\r')
        column = 0;
    else if (c == '\b')
        terminal_backspace();
    else
    {
        text_buffer[row][column] = c;
        color_buffer[row][column] = color;
        if (use_framebuffer) framebuffer_draw_cell(row, column);
        else VGA_MEMORY[row * VGA_WIDTH + column] = vga_entry(c, color);
        column++;
        if (column >= columns)
        {
            column = 0;
            row++;
            if (row >= rows)
            {
                terminal_scroll();
                redraw_all = 1;
            }
        }
    }

    if (use_framebuffer)
    {
        /* Only an actual scroll requires repainting every cell; a plain
         * newline, backspace, or carriage return already updated the one
         * cell that changed above. */
        if (redraw_all) framebuffer_redraw_all();
        cursor_visible = 1;
        framebuffer_draw_cursor();
    }
    else update_hardware_cursor();
    mouse_cursor_draw();
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
    if (use_framebuffer) framebuffer_hide_cursor();
    if (new_row >= rows) new_row = rows - 1;
    if (new_column >= columns) new_column = columns - 1;
    row = new_row;
    column = new_column;
    cursor_visible = 1;
    if (use_framebuffer) framebuffer_draw_cursor();
    else update_hardware_cursor();
}

void terminal_cursor_left(void)
{
    if (use_framebuffer) framebuffer_hide_cursor();
    if (column > 0) column--;
    else if (row > 0) { row--; column = columns - 1; }
    if (use_framebuffer) framebuffer_draw_cursor(); else update_hardware_cursor();
}

void terminal_cursor_right(void)
{
    if (use_framebuffer) framebuffer_hide_cursor();
    if (column + 1 < columns) column++;
    else if (row + 1 < rows) { row++; column = 0; }
    if (use_framebuffer) framebuffer_draw_cursor(); else update_hardware_cursor();
}

void terminal_cursor_home(void)
{
    if (use_framebuffer) framebuffer_hide_cursor();
    column = 0;
    if (use_framebuffer) framebuffer_draw_cursor(); else update_hardware_cursor();
}

void terminal_cursor_blink_tick(void)
{
    if (!use_framebuffer) return;
    cursor_ticks++;
    if (cursor_ticks < 50) return;
    cursor_ticks = 0;
    if (cursor_visible)
    {
        framebuffer_hide_cursor();
        cursor_visible = 0;
    }
    else
    {
        cursor_visible = 1;
        framebuffer_draw_cursor();
    }
}

size_t terminal_width(void) { return columns; }
size_t terminal_height(void) { return rows; }