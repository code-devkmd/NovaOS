#include "idt.h"

#include <stdint.h>

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void irq0(void); 
extern void irq1(void); 
extern void irq2(void); 
extern void irq3(void); 
extern void irq4(void); 
extern void irq5(void); 
extern void irq6(void); 
extern void irq7(void); 
extern void irq8(void); 
extern void irq9(void); 
extern void irq10(void); 
extern void irq11(void); 
extern void irq12(void); 
extern void irq13(void); 
extern void irq14(void); 
extern void irq15(void);

struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idt_pointer;

static void idt_set_gate(
    int number,
    uint32_t base,
    uint16_t selector,
    uint8_t flags
)
{
    idt[number].base_low = base & 0xFFFF;
    idt[number].base_high = (base >> 16) & 0xFFFF;

    idt[number].selector = selector;
    idt[number].zero = 0;
    idt[number].flags = flags;
}

void idt_init(void)
{
    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0, 0x08, 0);
    }

    idt_set_gate(0,  (uint32_t)isr0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  0x08, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2,  0x08, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3,  0x08, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4,  0x08, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5,  0x08, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6,  0x08, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7,  0x08, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8,  0x08, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E); 
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E); 
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E); 
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E); 
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E); 
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E); 
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E); 
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E); 
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E); 
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E); 
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E); 
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E); 
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E); 
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E); 
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E); 
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idt_pointer)
    );
}

