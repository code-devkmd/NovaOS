#include "io.h"
#include "terminal.h"

void print(const char *str)
{
    terminal_write(str);
}

void print_int(int num)
{
    char buffer[12];
    unsigned int value;
    int i = 0;

    if (num == 0)
    {
        terminal_putchar('0');
        return;
    }

    if (num < 0)
    {
        terminal_putchar('-');
        value = (unsigned int)(-(num + 1)) + 1;
    }
    else
    {
        value = (unsigned int)num;
    }

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0)
    {
        terminal_putchar(buffer[--i]);
    }
}
