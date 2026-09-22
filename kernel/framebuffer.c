#include "framebuffer.h"
#include "serial.h"
#include "memory.h"
#include <stdint.h>

extern const uint8_t font8x8[95][8];

#define MULTIBOOT_MAGIC 0x2BADB002U
#define MULTIBOOT_FLAG_FRAMEBUFFER (1U << 12)
/* Per the Multiboot v1 spec: 0 = indexed color, 1 = direct RGB color,
 * 2 = EGA-standard text mode. */
#define FRAMEBUFFER_TYPE_INDEXED 0U
#define FRAMEBUFFER_TYPE_RGB 1U

struct multiboot_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t color_info;
};

static volatile uint8_t *framebuffer;
static uint32_t fb_width;
static uint32_t fb_height;
static uint32_t fb_pitch;
static uint8_t fb_bpp;
static uint8_t fb_type;
static uint8_t fb_red_pos;
static uint8_t fb_red_mask;
static uint8_t fb_green_pos;
static uint8_t fb_green_mask;
static uint8_t fb_blue_pos;
static uint8_t fb_blue_mask;
static int available;

static uint32_t scale_channel(uint8_t value, uint8_t mask)
{
    uint32_t max = (1U << mask) - 1U;
    if (max == 0) return 0;
    return ((uint32_t)value * max + 127U) / 255U;
}

static uint32_t pack_rgb(uint32_t color)
{
    uint8_t r = (uint8_t)(color >> 16);
    uint8_t g = (uint8_t)(color >> 8);
    uint8_t b = (uint8_t)color;
    uint32_t pixel = 0;

    pixel |= scale_channel(r, fb_red_mask) << fb_red_pos;
    pixel |= scale_channel(g, fb_green_mask) << fb_green_pos;
    pixel |= scale_channel(b, fb_blue_mask) << fb_blue_pos;
    return pixel;
}

int framebuffer_init(uint32_t multiboot_info_address)
{
    struct multiboot_info *info = (struct multiboot_info *)multiboot_info_address;
    uint64_t address;

    available = 0;

    if (!info || !(info->flags & MULTIBOOT_FLAG_FRAMEBUFFER))
    {
        serial_write("[GFX] GRUB did not provide a framebuffer\n");
        return -1;
    }

    address = info->framebuffer_addr;
    if ((address >> 32) != 0)
    {
        serial_write("[GFX] Framebuffer address is above 4 GiB\n");
        return -1;
    }

    if (info->framebuffer_width == 0 || info->framebuffer_height == 0 ||
        info->framebuffer_pitch == 0)
        return -1;

    if (info->framebuffer_type != FRAMEBUFFER_TYPE_RGB ||
        (info->framebuffer_bpp != 32 && info->framebuffer_bpp != 24))
    {
        serial_write("[GFX] Unsupported framebuffer format\n");
        return -1;
    }

    framebuffer = (volatile uint8_t *)(uintptr_t)(uint32_t)address;
    fb_width = info->framebuffer_width;
    fb_height = info->framebuffer_height;
    fb_pitch = info->framebuffer_pitch;
    fb_bpp = info->framebuffer_bpp;
    fb_type = info->framebuffer_type;

    /* Multiboot v1's RGB color information is stored after the type byte. */
    {
        const uint8_t *color = (const uint8_t *)((uintptr_t)multiboot_info_address + 110U);
        fb_red_pos = color[0];
        fb_red_mask = color[1];
        fb_green_pos = color[2];
        fb_green_mask = color[3];
        fb_blue_pos = color[4];
        fb_blue_mask = color[5];
    }

    /* Some BIOS/VBE implementations (notably QEMU's "std" VGA card) report a
     * degenerate color descriptor for 32bpp modes, e.g. a zero-width red
     * channel. A zero mask silently drops that channel from every color we
     * draw, so fall back to the near-universal 0xXXRRGGBB byte layout
     * (8/8/8 bits at positions 16/8/0) whenever the descriptor looks bogus. */
    if (fb_red_mask == 0 || fb_green_mask == 0 || fb_blue_mask == 0)
    {
        serial_write("[GFX] Bogus color descriptor from firmware, using default RGB888 layout\n");
        fb_red_pos = 16; fb_red_mask = 8;
        fb_green_pos = 8; fb_green_mask = 8;
        fb_blue_pos = 0; fb_blue_mask = 8;
    }

    available = 1;

    serial_write("[GFX] Framebuffer enabled: ");
    serial_write_dec(fb_width);
    serial_write("x");
    serial_write_dec(fb_height);
    serial_write("x");
    serial_write_dec(fb_bpp);
    serial_write("\n");

    return 0;
}

int framebuffer_available(void) { return available; }
uint32_t framebuffer_width(void) { return fb_width; }
uint32_t framebuffer_height(void) { return fb_height; }
uint32_t framebuffer_pitch(void) { return fb_pitch; }
uint8_t framebuffer_bpp(void) { return fb_bpp; }

void framebuffer_putpixel(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t packed;
    volatile uint8_t *pixel;

    if (!available || x >= fb_width || y >= fb_height) return;

    packed = pack_rgb(color);
    pixel = framebuffer + y * fb_pitch + x * (fb_bpp / 8U);

    if (fb_bpp == 32)
        *(volatile uint32_t *)pixel = packed;
    else
    {
        pixel[0] = (uint8_t)(packed & 0xFF);
        pixel[1] = (uint8_t)((packed >> 8) & 0xFF);
        pixel[2] = (uint8_t)((packed >> 16) & 0xFF);
    }
}

void framebuffer_clear(uint32_t color)
{
    uint32_t packed, y, x;
    if (!available) return;
    packed = pack_rgb(color);

    if (fb_bpp == 32)
    {
        for (y = 0; y < fb_height; y++)
        {
            volatile uint32_t *row = (volatile uint32_t *)(framebuffer + y * fb_pitch);
            for (x = 0; x < fb_width; x++) row[x] = packed;
        }
    }
    else
    {
        for (y = 0; y < fb_height; y++)
            for (x = 0; x < fb_width; x++)
            {
                volatile uint8_t *pixel = framebuffer + y * fb_pitch + x * 3U;
                pixel[0] = (uint8_t)(packed & 0xFF);
                pixel[1] = (uint8_t)((packed >> 8) & 0xFF);
                pixel[2] = (uint8_t)((packed >> 16) & 0xFF);
            }
    }
}

void framebuffer_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    uint32_t yy, xx, packed;
    if (!available || width == 0 || height == 0) return;
    if (x >= fb_width || y >= fb_height) return;
    if (width > fb_width - x) width = fb_width - x;
    if (height > fb_height - y) height = fb_height - y;
    packed = pack_rgb(color);

    if (fb_bpp == 32)
    {
        for (yy = 0; yy < height; yy++)
        {
            volatile uint32_t *row = (volatile uint32_t *)(framebuffer + (y + yy) * fb_pitch) + x;
            for (xx = 0; xx < width; xx++) row[xx] = packed;
        }
    }
    else
    {
        for (yy = 0; yy < height; yy++)
            for (xx = 0; xx < width; xx++)
            {
                volatile uint8_t *pixel = framebuffer + (y + yy) * fb_pitch + (x + xx) * 3U;
                pixel[0] = (uint8_t)(packed & 0xFF);
                pixel[1] = (uint8_t)((packed >> 8) & 0xFF);
                pixel[2] = (uint8_t)((packed >> 16) & 0xFF);
            }
    }
}

void framebuffer_draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    uint32_t xx;
    uint32_t yy;
    if (!available || width == 0 || height == 0) return;
    for (xx = 0; xx < width; xx++)
    {
        framebuffer_putpixel(x + xx, y, color);
        framebuffer_putpixel(x + xx, y + height - 1, color);
    }
    for (yy = 0; yy < height; yy++)
    {
        framebuffer_putpixel(x, y + yy, color);
        framebuffer_putpixel(x + width - 1, y + yy, color);
    }
}

void framebuffer_draw_char(uint32_t x, uint32_t y, char c, uint32_t fg, uint32_t bg)
{
    int index = (int)(unsigned char)c - 32;
    uint32_t yy;
    uint32_t xx;
    uint32_t sy;
    uint32_t sx;
    const uint8_t *glyph;

    if (!available) return;
    if (index < 0 || index >= 95) index = '?' - 32;
    glyph = font8x8[index];

    /* Each source glyph pixel becomes a FONT_SCALE x FONT_SCALE block so
     * text stays readable at high resolutions. */
    for (yy = 0; yy < 8; yy++)
        for (xx = 0; xx < 8; xx++)
        {
            uint32_t px_color = (glyph[yy] & (1U << (7U - xx))) ? fg : bg;
            for (sy = 0; sy < FONT_SCALE; sy++)
                for (sx = 0; sx < FONT_SCALE; sx++)
                    framebuffer_putpixel(x + xx * FONT_SCALE + sx,
                                          y + yy * FONT_SCALE + sy, px_color);
        }
}

void framebuffer_draw_text(uint32_t x, uint32_t y, const char *text, uint32_t fg, uint32_t bg)
{
    uint32_t start_x = x;
    if (!text) return;

    while (*text)
    {
        if (*text == '\n')
        {
            x = start_x;
            y += 8U * FONT_SCALE;
        }
        else
        {
            framebuffer_draw_char(x, y, *text, fg, bg);
            x += 8U * FONT_SCALE;
        }
        text++;
    }
}