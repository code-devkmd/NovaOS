#include "serial.h"

#define COM1 0x3F8

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static int serial_transmit_empty(void)
{
    return inb(COM1 + 5) & 0x20;
}

void serial_init(void)
{
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

void serial_putchar(char c)
{
    while (!serial_transmit_empty()) {}
    outb(COM1, (uint8_t)c);
}

void serial_write(const char *str)
{
    if (!str) return;
    while (*str) serial_putchar(*str++);
}

void serial_write_hex(uint32_t value)
{
    const char *digits = "0123456789ABCDEF";
    serial_write("0x");

    for (int shift = 28; shift >= 0; shift -= 4)
        serial_putchar(digits[(value >> shift) & 0xF]);
}

void serial_write_dec(uint32_t value)
{
    char buffer[11];
    int i = 10;
    buffer[10] = 0;
    if (value == 0) { serial_write("0"); return; }
    while (value > 0 && i > 0)
    {
        buffer[--i] = (char)('0' + (value % 10));
        value /= 10;
    }
    serial_write(&buffer[i]);
}
