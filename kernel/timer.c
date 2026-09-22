#include "timer.h"
#include "pic.h"
#include "terminal.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_BASE_FREQUENCY 1193182U

static volatile uint32_t ticks;

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void timer_init(void)
{
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / TIMER_FREQUENCY);

    ticks = 0;

    /* Channel 0, lobyte/hibyte, rate generator, binary mode. */
    outb(PIT_COMMAND, 0x34);
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    /* IRQ0 was kept masked until the PIT was initialized. */
    pic_clear_mask(0);
}

void timer_irq_handler(void)
{
    ticks++;
    terminal_cursor_blink_tick();
}

uint32_t timer_get_ticks(void)
{
    return ticks;
}

uint32_t timer_get_uptime_seconds(void)
{
    return ticks / TIMER_FREQUENCY;
}
