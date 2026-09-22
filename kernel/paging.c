#include "paging.h"
#include "memory.h"
#include "serial.h"

#define PAGE_SIZE_4MB 0x00400000U
#define PAGE_DIRECTORY_ENTRIES 1024U
#define PAGE_SIZE_FLAG 0x080U
#define CR4_PSE 0x00000010U

static uint32_t page_directory_address;
static int paging_active;

static void clear_directory(uint32_t *directory)
{
    uint32_t i;
    for (i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
        directory[i] = 0;
}

static void load_page_directory(uint32_t address)
{
    __asm__ volatile ("mov %0, %%cr3" : : "r"(address) : "memory");
}

static void enable_4mb_pages(void)
{
    uint32_t cr4;
    __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= CR4_PSE;
    __asm__ volatile ("mov %0, %%cr4" : : "r"(cr4) : "memory");
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
    uint32_t directory_frame;
    uint32_t i;

    if (memory_total_frames() == 0)
        return -1;

    directory_frame = (uint32_t)frame_alloc();
    if (directory_frame == 0)
        return -1;

    page_directory_address = directory_frame;
    directory = (uint32_t *)(uintptr_t)directory_frame;
    clear_directory(directory);

    /* Identity-map the complete 32-bit address space with 4 MiB pages.
       This keeps GRUB's framebuffer and future MMIO regions accessible. */
    for (i = 0; i < PAGE_DIRECTORY_ENTRIES; i++)
        directory[i] = (i * PAGE_SIZE_4MB) | PAGE_PRESENT | PAGE_WRITABLE | PAGE_SIZE_FLAG;

    enable_4mb_pages();
    load_page_directory(page_directory_address);
    enable_paging_cpu();
    paging_active = 1;

    serial_write("[PAGING] 4 MiB identity mapping enabled for 4 GiB address space\n");
    return 0;
}

int paging_enabled(void) { return paging_active; }
uint32_t paging_directory_address(void) { return page_directory_address; }
