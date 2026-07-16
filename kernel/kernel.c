#include "terminal.h"
#include "io.h"
#include "keyboard.h"

void kernel_main(void)
{
    terminal_setcolor(0x1F);

    terminal_clear();

    terminal_write("========================================\n");
    terminal_write("            NovaOS v0.2\n");
    terminal_write("========================================\n\n");

    terminal_setcolor(0x0A);

    terminal_write("Welcome to NovaOS!\n");

    terminal_setcolor(0x0F);

    terminal_write("This is my operating system.\n\n");

    int a = 28;
    print("Data: ");
    print_int(a);

    print("Sum of 100 and 28 is: ");
    print_int(100 + 28);

    terminal_clear();

    terminal_write("Type something: ");

    while (1)
    {
        char c = keyboard_getchar();

        if (c)
            terminal_putchar(c);
    }

    while (1);
}