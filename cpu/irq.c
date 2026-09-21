#include "irq.h"
#include "pic.h"
#include "keyboard.h"
#include "timer.h"

void irq_handler(unsigned int irq)
{
    if (irq == 0)
        timer_irq_handler();
    else if (irq == 1)
        keyboard_irq_handler();

    if (irq >= 8)
    {
        /* Slave PIC must receive EOI first. */
        __asm__ volatile ("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0xA0));
    }

    /* Acknowledge the master PIC. */
    __asm__ volatile ("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}

void irq_init(void)
{
    /* Keep only timer (IRQ0) and keyboard (IRQ1) enabled for now. */
    pic_set_mask(2);
    pic_set_mask(3);
    pic_set_mask(4);
    pic_set_mask(5);
    pic_set_mask(6);
    pic_set_mask(7);

    for (unsigned int irq = 8; irq < 16; irq++)
        pic_set_mask((unsigned char)irq);

    /* IRQ0 remains masked until timer_init() programs the PIT. */
    pic_set_mask(0);
    pic_clear_mask(1);
}
