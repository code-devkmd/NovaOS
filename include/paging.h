#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_PRESENT 0x001U
#define PAGE_WRITABLE 0x002U
#define PAGE_USER 0x004U

int paging_init(void);
int paging_enabled(void);
uint32_t paging_directory_address(void);

#endif
