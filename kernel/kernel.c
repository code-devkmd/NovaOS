#include "terminal.h"
#include "io.h"
#include "keyboard.h"

void kernel_main(void)
{
    terminal_setcolor(0x1F);

    terminal_clear();

    terminal_write("========================================\n");
    terminal_write("            NovaOS v0.3\n");
    terminal_write("========================================\n\n");

    terminal_setcolor(0x0A);

    terminal_write("Welcome to NovaOS!\n");

    terminal_setcolor(0x0F);

    terminal_write("This is my operating system.\n\n");

    terminal_write("\nType something: ");

    char input[80];
    size_t index = 0;

    while (1)
    {
        char c = keyboard_getchar();

        if (!c)
            continue;

        if (c == '\b')
        {
            if (index > 0)
            {
                index--;
                input[index] = '\0';
                terminal_putchar('\b');
            }
        }
        else if (c == '\n')
        {
            terminal_putchar('\n');
            input[index] = '\0';

            if (index > 0)
            {
                terminal_write("You typed: ");
                terminal_write(input);
                terminal_putchar('\n');
            }

            index = 0;
            terminal_write("Type something: ");
        }
        else if (index < 79)
        {
            input[index++] = c;
            terminal_putchar(c);
        }
    }
}