#include "terminal.h"

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

    terminal_write("This is my operating system.\n");

    while (1);
}