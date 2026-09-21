#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_info_address);

#endif
