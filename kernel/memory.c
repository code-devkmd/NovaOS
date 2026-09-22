#include "memory.h"
#include "serial.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002U
#define MULTIBOOT_INFO_MEMORY_MAP  (1U << 6)
#define MULTIBOOT_MEMORY_AVAILABLE 1U

#define BITMAP_BYTES (MAX_PHYSICAL_PAGES / 8U)

struct multiboot_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint8_t  syms[16];
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
} __attribute__((packed));

struct multiboot_mmap_entry
{
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

extern uint8_t kernel_end;

static uint8_t frame_bitmap[BITMAP_BYTES];
static uint32_t total_frames;
static uint32_t free_frames;
static uint32_t highest_frame;

static void bitmap_set(uint32_t frame)
{
    frame_bitmap[frame >> 3] |= (uint8_t)(1U << (frame & 7U));
}

static void bitmap_clear(uint32_t frame)
{
    frame_bitmap[frame >> 3] &= (uint8_t)~(1U << (frame & 7U));
}

static int bitmap_test(uint32_t frame)
{
    return (frame_bitmap[frame >> 3] & (uint8_t)(1U << (frame & 7U))) != 0;
}

static uint32_t address_to_frame(uint64_t address)
{
    return (uint32_t)(address / PAGE_SIZE);
}

static void reserve_range(uint64_t start, uint64_t length)
{
    uint64_t end = start + length;
    uint32_t first = address_to_frame(start);
    uint32_t last = (uint32_t)((end + PAGE_SIZE - 1U) / PAGE_SIZE);

    if (first >= MAX_PHYSICAL_PAGES)
        return;

    if (last > MAX_PHYSICAL_PAGES)
        last = MAX_PHYSICAL_PAGES;

    while (first < last)
    {
        if (!bitmap_test(first))
        {
            bitmap_set(first);
            if (free_frames > 0)
                free_frames--;
        }
        first++;
    }
}

static void release_range(uint64_t start, uint64_t length)
{
    uint64_t end = start + length;
    uint32_t first = (uint32_t)((start + PAGE_SIZE - 1U) / PAGE_SIZE);
    uint32_t last = (uint32_t)(end / PAGE_SIZE);

    if (first >= MAX_PHYSICAL_PAGES)
        return;

    if (last > MAX_PHYSICAL_PAGES)
        last = MAX_PHYSICAL_PAGES;

    while (first < last)
    {
        if (bitmap_test(first))
        {
            bitmap_clear(first);
            free_frames++;
        }
        first++;
    }
}

int memory_init(uint32_t multiboot_magic, uint32_t multiboot_info_address)
{
    uint32_t i;
    struct multiboot_info *info;
    uint32_t kernel_end_address = (uint32_t)&kernel_end;

    for (i = 0; i < BITMAP_BYTES; i++)
        frame_bitmap[i] = 0xFF;

    total_frames = 0;
    free_frames = 0;
    highest_frame = 0;

    if (multiboot_magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        serial_write("[MEM] Invalid Multiboot magic\n");
        return -1;
    }

    info = (struct multiboot_info *)multiboot_info_address;

    if ((info->flags & MULTIBOOT_INFO_MEMORY_MAP) == 0)
    {
        serial_write("[MEM] Multiboot memory map unavailable\n");
        return -1;
    }

    {
        uint32_t offset = 0;
        while (offset < info->mmap_length)
        {
            struct multiboot_mmap_entry *entry =
                (struct multiboot_mmap_entry *)(info->mmap_addr + offset);
            uint64_t start = entry->addr;
            uint64_t length = entry->len;

            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && length != 0)
            {
                uint64_t end = start + length;
                uint32_t last = (uint32_t)((end + PAGE_SIZE - 1U) / PAGE_SIZE);

                if (last > highest_frame)
                    highest_frame = last;

                release_range(start, length);
            }

            offset += entry->size + sizeof(entry->size);
        }
    }

    if (highest_frame > MAX_PHYSICAL_PAGES)
        highest_frame = MAX_PHYSICAL_PAGES;

    total_frames = highest_frame;

    /* Never give the allocator memory below 1 MiB. */
    reserve_range(0, 0x100000);

    /* Reserve the complete kernel image, including its bootstrap stack. */
    reserve_range(0x100000, (uint64_t)kernel_end_address - 0x100000U);

    /* GRUB's framebuffer is physical memory too; never hand it to the
       allocator or paging/heap users may overwrite the display. */
    if (info->flags & (1U << 12))
    {
        struct multiboot_info *fb_info = info;
        uint64_t framebuffer_address = fb_info->framebuffer_addr;
        uint64_t framebuffer_length =
            (uint64_t)fb_info->framebuffer_pitch * fb_info->framebuffer_height;

        if (framebuffer_address != 0 && framebuffer_length != 0)
        {
            reserve_range(framebuffer_address, framebuffer_length);
            serial_write("[MEM] Framebuffer memory reserved\n");
        }
    }

    /* The bitmap itself lives inside the kernel image's BSS. */
    reserve_range((uint32_t)frame_bitmap, sizeof(frame_bitmap));

    serial_write("[MEM] Physical frame allocator initialized\n");
    return 0;
}

void *frame_alloc(void)
{
    uint32_t frame;

    for (frame = 0; frame < total_frames; frame++)
    {
        if (!bitmap_test(frame))
        {
            bitmap_set(frame);
            if (free_frames > 0)
                free_frames--;
            return (void *)(frame * PAGE_SIZE);
        }
    }

    return 0;
}

void frame_free(void *address)
{
    uint32_t frame = (uint32_t)address / PAGE_SIZE;

    if (frame >= total_frames)
        return;

    if (bitmap_test(frame))
    {
        bitmap_clear(frame);
        free_frames++;
    }
}

uint32_t memory_total_bytes(void)
{
    return total_frames * PAGE_SIZE;
}

uint32_t memory_free_bytes(void)
{
    return free_frames * PAGE_SIZE;
}

uint32_t memory_used_bytes(void)
{
    return memory_total_bytes() - memory_free_bytes();
}

uint32_t memory_total_frames(void)
{
    return total_frames;
}

uint32_t memory_free_frames(void)
{
    return free_frames;
}
