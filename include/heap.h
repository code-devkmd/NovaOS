#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

int heap_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);
size_t heap_used(void);
size_t heap_free(void);

#endif
