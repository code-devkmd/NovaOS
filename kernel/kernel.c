#include "terminal.h"
#include "io.h"
#include "keyboard.h"
#include "shell.h"

void kernel_main(void)
{
    terminal_setcolor(0x1F);

    terminal_clear();

    terminal_write("========================================\n");
    terminal_write("            NovaOS v0.4\n");
    terminal_write("========================================\n\n");

    terminal_setcolor(0x0A);

    terminal_write("Welcome to NovaOS!\n");

    terminal_setcolor(0x0F);

    shell_start();
    shell_run();
}