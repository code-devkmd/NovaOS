#include "io.h"
#include "terminal.h"

void print(const char *str)
{
    terminal_write(str);
}

void print_int(int num)
{
    char buffer[12];
    int i = 0;

    if (num == 0)
    {
        terminal_putchar('0');
        return;
    }

    if (num < 0)
    {
        terminal_putchar('-');
        num = -num;
    }

    while (num > 0)
    {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0)
    {
        terminal_putchar(buffer[--i]);
    }
}