#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define PAGE_SIZE 4096U
#define MAX_PHYSICAL_PAGES 1048576U

int memory_init(uint32_t multiboot_magic, uint32_t multiboot_info_address);
void *frame_alloc(void);
void frame_free(void *address);
uint32_t memory_total_bytes(void);
uint32_t memory_free_bytes(void);
uint32_t memory_used_bytes(void);
uint32_t memory_total_frames(void);
uint32_t memory_free_frames(void);

#endif
