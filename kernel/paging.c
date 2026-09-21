#include "paging.h"
#include "memory.h"
#include "serial.h"

#define PAGE_TABLE_ENTRIES 1024U
#define INITIAL_MAP_MB 64U
#define INITIAL_MAP_PAGES ((INITIAL_MAP_MB * 1024U * 1024U) / PAGE_SIZE)

static uint32_t page_directory_address;
static int paging_active;

static void clear_page(uint32_t *page)
{
    uint32_t i;
    for (i = 0; i < PAGE_TABLE_ENTRIES; i++)
        page[i] = 0;
}

static void load_page_directory(uint32_t address)
{
    __asm__ volatile ("mov %0, %%cr3" : : "r"(address) : "memory");
}

static void enable_paging_cpu(void)
{
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000U;
    __asm__ volatile ("mov %0, %%cr0" : : "r"(cr0) : "memory");
}

int paging_init(void)
{
    uint32_t *directory;
    uint32_t mapped_pages = INITIAL_MAP_PAGES;
    uint32_t page;
    uint32_t directory_frame;

    if (memory_total_frames() == 0)
        return -1;

    if (mapped_pages > memory_total_frames())
        mapped_pages = memory_total_frames();

    directory_frame = (uint32_t)frame_alloc();
    if (directory_frame == 0)
        return -1;

    page_directory_address = directory_frame;
    directory = (uint32_t *)directory_frame;
    clear_page(directory);

    for (page = 0; page < mapped_pages; page += PAGE_TABLE_ENTRIES)
    {
        uint32_t table_frame = (uint32_t)frame_alloc();
        uint32_t *table;
        uint32_t pde_index = page / PAGE_TABLE_ENTRIES;
        uint32_t i;

        if (table_frame == 0)
            return -1;

        table = (uint32_t *)table_frame;
        clear_page(table);

        for (i = 0; i < PAGE_TABLE_ENTRIES && (page + i) < mapped_pages; i++)
        {
            uint32_t physical = (page + i) * PAGE_SIZE;
            table[i] = physical | PAGE_PRESENT | PAGE_WRITABLE;
        }

        directory[pde_index] = table_frame | PAGE_PRESENT | PAGE_WRITABLE;
    }

    load_page_directory(page_directory_address);
    enable_paging_cpu();
    paging_active = 1;

    serial_write("[PAGING] Identity paging enabled (first 64 MiB or available memory)\n");
    return 0;
}

int paging_enabled(void)
{
    return paging_active;
}

uint32_t paging_directory_address(void)
{
    return page_directory_address;
}
