#include "mouse.h"
#include "pic.h"
#include "serial.h"
#include "graphics.h"
#include "terminal.h"
#include <stdint.h>

#define MOUSE_DATA       0x60
#define MOUSE_STATUS     0x64
#define MOUSE_COMMAND    0x64

#define STATUS_OUTPUT_FULL 0x01
#define STATUS_INPUT_FULL  0x02
#define STATUS_AUX_DATA    0x20

#define CMD_ENABLE_AUX     0xA8
#define CMD_READ_CONFIG    0x20
#define CMD_WRITE_CONFIG   0x60
#define CMD_WRITE_AUX      0xD4

#define MOUSE_SET_DEFAULTS 0xF6
#define MOUSE_ENABLE        0xF4
#define MOUSE_ACK            0xFA

static int available;
static int x;
static int y;
static int cursor_drawn;
static uint8_t buttons;
static uint8_t packet[3];
static uint8_t packet_index;

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static int wait_input_clear(void)
{
    uint32_t timeout = 100000;
    while (timeout--)
    {
        if ((inb(MOUSE_STATUS) & STATUS_INPUT_FULL) == 0)
            return 0;
    }
    return -1;
}

static int wait_output_full(void)
{
    uint32_t timeout = 100000;
    while (timeout--)
    {
        if (inb(MOUSE_STATUS) & STATUS_OUTPUT_FULL)
            return 0;
    }
    return -1;
}

static int write_controller(uint8_t command)
{
    if (wait_input_clear() != 0)
        return -1;
    outb(MOUSE_COMMAND, command);
    return 0;
}

static int write_mouse(uint8_t value)
{
    if (write_controller(CMD_WRITE_AUX) != 0)
        return -1;
    if (wait_input_clear() != 0)
        return -1;
    outb(MOUSE_DATA, value);
    return 0;
}

static int read_mouse(uint8_t *value)
{
    if (!value || wait_output_full() != 0)
        return -1;
    *value = inb(MOUSE_DATA);
    return 0;
}

static int mouse_command_expect_ack(uint8_t command)
{
    uint8_t response;
    if (write_mouse(command) != 0)
        return -1;
    if (read_mouse(&response) != 0 || response != MOUSE_ACK)
        return -1;
    return 0;
}

#define CURSOR_WIDTH 12
#define CURSOR_HEIGHT 18
/* The 12x18 bitmap below is tiny and hard to see on a 1024x768 screen, so
 * draw each of its pixels as a CURSOR_SCALE x CURSOR_SCALE block. The
 * hide/redraw region below must use the same scaled size or it will only
 * erase part of the cursor and leave visible trails behind it. */
#define CURSOR_SCALE 2
#define CURSOR_DRAW_WIDTH  (CURSOR_WIDTH * CURSOR_SCALE)
#define CURSOR_DRAW_HEIGHT (CURSOR_HEIGHT * CURSOR_SCALE)

static const uint8_t cursor_shape[CURSOR_HEIGHT][CURSOR_WIDTH] =
{
    {2,0,0,0,0,0,0,0,0,0,0,0},
    {2,2,0,0,0,0,0,0,0,0,0,0},
    {2,1,2,0,0,0,0,0,0,0,0,0},
    {2,1,1,2,0,0,0,0,0,0,0,0},
    {2,1,1,1,2,0,0,0,0,0,0,0},
    {2,1,1,1,1,2,0,0,0,0,0,0},
    {2,1,1,1,1,1,2,0,0,0,0,0},
    {2,1,1,1,1,1,1,2,0,0,0,0},
    {2,1,1,1,1,1,1,1,2,0,0,0},
    {2,1,1,1,1,1,1,1,1,2,0,0},
    {2,1,1,1,1,1,1,1,1,1,2,0},
    {2,1,1,1,1,1,1,1,1,1,1,2},
    {2,1,1,1,1,1,1,1,1,2,2,0},
    {2,1,1,1,1,1,1,1,2,0,0,0},
    {2,1,1,1,1,1,1,2,0,0,0,0},
    {2,1,1,1,1,1,2,0,0,0,0,0},
    {2,1,1,1,1,2,0,0,0,0,0,0},
    {2,1,1,1,2,0,0,0,0,0,0,0}
};

static int clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void mouse_irq_handler(void)
{
    uint8_t status;
    uint8_t value;

    status = inb(MOUSE_STATUS);
    if ((status & STATUS_OUTPUT_FULL) == 0)
        return;

    /* Ignore controller bytes that are not from the auxiliary device. */
    if ((status & STATUS_AUX_DATA) == 0)
    {
        (void)inb(MOUSE_DATA);
        return;
    }

    value = inb(MOUSE_DATA);

    if (packet_index == 0 && (value & 0x08) == 0)
        return;

    packet[packet_index++] = value;
    if (packet_index < 3)
        return;

    packet_index = 0;

    /* Discard packets with x/y overflow. */
    if (packet[0] & 0xC0)
        return;

    {
        int dx = (int)(int8_t)packet[1];
        int dy = (int)(int8_t)packet[2];

        mouse_cursor_hide();
        x += dx;
        y -= dy;

        if (graphics_available())
        {
            x = clamp(x, 0, (int)graphics_width() - 1);
            y = clamp(y, 0, (int)graphics_height() - 1);
        }
        else
        {
            x = clamp(x, 0, 79);
            y = clamp(y, 0, 24);
        }
        buttons = packet[0] & 0x07;
        mouse_cursor_draw();
    }
}

void mouse_cursor_hide(void)
{
    /* NovaOS v2.1 uses an overlay cursor. Terminal redraws underneath it,
       so hiding simply clears the old cursor area to the desktop color. */
    if (!cursor_drawn || !graphics_available())
        return;

    terminal_redraw_region((uint32_t)x, (uint32_t)y, CURSOR_DRAW_WIDTH, CURSOR_DRAW_HEIGHT);
    cursor_drawn = 0;
}

void mouse_cursor_draw(void)
{
    uint32_t yy, xx;
    if (!available || !graphics_available())
        return;

    for (yy = 0; yy < CURSOR_HEIGHT; yy++)
        for (xx = 0; xx < CURSOR_WIDTH; xx++)
        {
            uint8_t pixel = cursor_shape[yy][xx];
            if (pixel == 1)
                graphics_fill_rect((uint32_t)x + xx * CURSOR_SCALE, (uint32_t)y + yy * CURSOR_SCALE,
                                    CURSOR_SCALE, CURSOR_SCALE, GFX_WHITE);
            else if (pixel == 2)
                graphics_fill_rect((uint32_t)x + xx * CURSOR_SCALE, (uint32_t)y + yy * CURSOR_SCALE,
                                    CURSOR_SCALE, CURSOR_SCALE, GFX_BLACK);
        }
    cursor_drawn = 1;
}

int mouse_init(void)
{
    uint8_t config;

    available = 0;
    packet_index = 0;
    buttons = 0;
    cursor_drawn = 0;
    if (graphics_available())
    {
        x = (int)graphics_width() / 2;
        y = (int)graphics_height() / 2;
    }
    else
    {
        x = 40;
        y = 12;
    }

    /* Enable the PS/2 auxiliary device. */
    if (write_controller(CMD_ENABLE_AUX) != 0)
        goto fail;

    /* Read the controller configuration byte. */
    if (write_controller(CMD_READ_CONFIG) != 0)
        goto fail;
    if (read_mouse(&config) != 0)
        goto fail;

    /* Enable IRQ12 and enable the mouse clock. */
    config |= 0x02;
    config &= (uint8_t)~0x20;

    if (write_controller(CMD_WRITE_CONFIG) != 0)
        goto fail;
    if (wait_input_clear() != 0)
        goto fail;
    outb(MOUSE_DATA, config);

    /* Reset the mouse to defaults and enable streaming packets. */
    if (mouse_command_expect_ack(MOUSE_SET_DEFAULTS) != 0)
        goto fail;
    if (mouse_command_expect_ack(MOUSE_ENABLE) != 0)
        goto fail;

    /* IRQ12 is delivered through the master's IRQ2 cascade. */
    pic_clear_mask(2);
    pic_clear_mask(12);
    available = 1;
    mouse_cursor_draw();

    serial_write("[MOUSE] PS/2 mouse initialized (IRQ12)\n");
    return 0;

fail:
    serial_write("[MOUSE] PS/2 mouse initialization failed\n");
    return -1;
}

int mouse_available(void) { return available; }
int mouse_x(void) { return x; }
int mouse_y(void) { return y; }
uint8_t mouse_buttons(void) { return buttons; }