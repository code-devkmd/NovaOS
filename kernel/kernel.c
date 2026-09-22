#include "terminal.h"
#include "io.h"
#include "keyboard.h"
#include "shell.h"
#include "kernel.h"
#include "idt.h"
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "pic.h"
#include "irq.h"
#include "serial.h"
#include "timer.h"
#include "memory.h"
#include "panic.h"
#include "paging.h"
#include "heap.h"
#include "ata.h"
#include "fs.h"
#include "graphics.h"
#include "mouse.h"

void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_address)
{
    serial_init();
    serial_write("[BOOT] NovaOS starting\n");

    gdt_init();
    idt_init();
    isr_init();
    pic_init();
    irq_init();
    keyboard_init();
    timer_init();
    if (graphics_init(multiboot_info_address) != 0)
        serial_write("[GFX] Falling back to VGA text mode\n");

    if (memory_init(multiboot_magic, multiboot_info_address) != 0)
        panic("Physical memory initialization failed");

    if (paging_init() != 0)
        panic("Paging initialization failed");

    if (heap_init() != 0)
        panic("Heap initialization failed");

    ata_init();
    if (fs_init() != 0)
        panic("Filesystem initialization failed");

    terminal_init();

    /* Mouse init draws the initial cursor, so it must run after
     * terminal_init(), which clears the whole framebuffer. Doing this
     * earlier meant the cursor was wiped out and only reappeared once
     * the mouse physically moved. */
    mouse_init();

    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
    terminal_write("========================================\n");
    terminal_write("            NovaOS v2.1\n");
    terminal_write("========================================\n\n");
    terminal_setcolor(VGA_COLOR_LIGHT_CYAN);

    terminal_setcolor(0x0A);
    terminal_write("Welcome to NovaOS!\n");
    terminal_setcolor(0x0F);

    __asm__ volatile ("sti");

    shell_start();    shell_run();
}