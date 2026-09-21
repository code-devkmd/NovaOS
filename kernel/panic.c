#include "panic.h"
#include "terminal.h"
#include "serial.h"

void panic(const char *message)
{
    terminal_setcolor(
        VGA_ENTRY_COLOR(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK)
    );

    terminal_write("\n\nKERNEL PANIC\n");
    terminal_write(message);
    terminal_write("\nSystem halted.\n");

    serial_write("\n[KERNEL PANIC] ");
    serial_write(message);
    serial_write("\n");

    __asm__ volatile ("cli");

    while (1)
        __asm__ volatile ("hlt");
}
