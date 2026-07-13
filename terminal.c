#include "terminal.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static volatile unsigned short *const VGA =
    (unsigned short *)0xB8000;

static size_t row = 0;
static size_t column = 0;

static unsigned char color = 0x0F;

void terminal_setcolor(unsigned char c)
{
    color = c;
}

void terminal_clear(void)
{
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            VGA[y * VGA_WIDTH + x] = (color << 8) | ' ';
        }
    }

    row = 0;
    column = 0;
}

void terminal_putchar(char c)
{
    if (c == '\n')
    {
        row++;
        column = 0;
        return;
    }

    VGA[row * VGA_WIDTH + column] =
        (color << 8) | c;

    column++;

    if (column >= VGA_WIDTH)
    {
        column = 0;
        row++;
    }

    if (row >= VGA_HEIGHT)
    {
        row = 0;
    }
}

void terminal_write(const char *str)
{
    while (*str)
    {
        terminal_putchar(*str++);
    }
}