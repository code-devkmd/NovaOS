#include "isr.h"
#include "terminal.h"
#include "serial.h"
#include "panic.h"

static const char *exception_messages[] =
{
    "Division By Zero", "Debug", "Non-Maskable Interrupt", "Breakpoint",
    "Overflow", "Bound Range Exceeded", "Invalid Opcode",
    "Device Not Available", "Double Fault", "Coprocessor Segment Overrun",
    "Invalid TSS", "Segment Not Present", "Stack-Segment Fault",
    "General Protection Fault", "Page Fault", "Reserved",
    "x87 Floating-Point Exception", "Alignment Check", "Machine Check",
    "SIMD Floating-Point Exception", "Virtualization Exception",
    "Control Protection Exception"
};

static uint32_t read_cr2(void)
{
    uint32_t value;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(value));
    return value;
}

void isr_handler(unsigned int number, unsigned int error_code)
{
    serial_write("\n[EXCEPTION] vector=");
    serial_write_hex(number);
    serial_write(" ");

    if (number < 22)
        serial_write(exception_messages[number]);
    else
        serial_write("Unknown");

    serial_write("\n");

    if (number == 14)
    {
        serial_write("[PAGE FAULT] address=");
        serial_write_hex(read_cr2());
        serial_write(" error=");
        serial_write_hex(error_code);
        serial_write("\n");
        panic("Page Fault");
    }

    if (number < 22)
        panic(exception_messages[number]);

    panic("Unknown CPU exception");
}

void isr_init(void)
{
}
