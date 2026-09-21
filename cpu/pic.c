#include "pic.h"

#include <stdint.h>

#define PIC1_COMMAND  0x20
#define PIC1_DATA     0x21

#define PIC2_COMMAND  0xA0
#define PIC2_DATA     0xA1

#define PIC_EOI       0x20

#define ICW1_INIT     0x10
#define ICW1_ICW4     0x01

#define ICW4_8086     0x01

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

static void io_wait(void)
{
    outb(0x80, 0);
}

void pic_init(void)
{
    uint8_t master_mask = inb(PIC1_DATA);
    uint8_t slave_mask = inb(PIC2_DATA);

    /*
     * Start initialization.
     */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    /*
     * Remap IRQs:
     *
     * Master: IRQ0-7  -> vectors 32-39
     * Slave:  IRQ8-15 -> vectors 40-47
     */
    outb(PIC1_DATA, 0x20);
    io_wait();

    outb(PIC2_DATA, 0x28);
    io_wait();

    /*
     * Tell the master that the slave is connected
     * through IRQ2.
     */
    outb(PIC1_DATA, 0x04);
    io_wait();

    /*
     * Tell the slave its cascade identity.
     */
    outb(PIC2_DATA, 0x02);
    io_wait();

    /*
     * Put both PICs into 8086 mode.
     */
    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    /*
     * Restore the interrupt masks.
     *
     * Interrupts remain disabled until we explicitly
     * enable them later.
     */
    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}


void pic_set_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        irq -= 8;
        port = PIC2_DATA;
    }

    value = inb(port) | (uint8_t)(1u << irq);
    outb(port, value);
}

void pic_clear_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        irq -= 8;
        port = PIC2_DATA;
    }

    value = inb(port) & (uint8_t)~(1u << irq);
    outb(port, value);
}
