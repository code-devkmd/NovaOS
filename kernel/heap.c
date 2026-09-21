#include "heap.h"
#include "serial.h"

#define HEAP_SIZE (2U * 1024U * 1024U)
#define ALIGNMENT 8U
#define MAGIC 0x4E484541U

typedef struct block
{
    size_t size;
    int free;
    unsigned int magic;
    struct block *next;
    struct block *prev;
} block_t;

static unsigned char heap_area[HEAP_SIZE] __attribute__((aligned(8)));
static block_t *first;
static size_t used_bytes;

static size_t align_up(size_t n)
{
    return (n + ALIGNMENT - 1U) & ~(ALIGNMENT - 1U);
}

static void split(block_t *b, size_t size)
{
    if (b->size < size + sizeof(block_t) + ALIGNMENT)
        return;

    block_t *n = (block_t *)((unsigned char *)(b + 1) + size);
    n->size = b->size - size - sizeof(block_t);
    n->free = 1;
    n->magic = MAGIC;
    n->next = b->next;
    n->prev = b;
    if (n->next)
        n->next->prev = n;
    b->next = n;
    b->size = size;
}

static void merge_next(block_t *b)
{
    block_t *n = b->next;
    if (!n || !n->free)
        return;
    b->size += sizeof(block_t) + n->size;
    b->next = n->next;
    if (b->next)
        b->next->prev = b;
}

int heap_init(void)
{
    first = (block_t *)heap_area;
    first->size = HEAP_SIZE - sizeof(block_t);
    first->free = 1;
    first->magic = MAGIC;
    first->next = 0;
    first->prev = 0;
    used_bytes = 0;
    serial_write("[HEAP] Kernel heap initialized\n");
    return 0;
}

void *kmalloc(size_t size)
{
    block_t *b;
    size = align_up(size);
    if (!size || !first)
        return 0;

    for (b = first; b; b = b->next)
    {
        if (b->free && b->size >= size)
        {
            split(b, size);
            b->free = 0;
            used_bytes += b->size;
            return (void *)(b + 1);
        }
    }
    return 0;
}

void kfree(void *ptr)
{
    block_t *b;
    if (!ptr)
        return;
    b = ((block_t *)ptr) - 1;
    if (b->magic != MAGIC || b->free)
        return;
    b->free = 1;
    if (used_bytes >= b->size)
        used_bytes -= b->size;
    if (b->prev && b->prev->free)
    {
        b = b->prev;
        merge_next(b);
    }
    merge_next(b);
}

size_t heap_used(void) { return used_bytes; }
size_t heap_free(void)
{
    size_t free_bytes = 0;
    block_t *b;
    for (b = first; b; b = b->next)
        if (b->free) free_bytes += b->size;
    return free_bytes;
}
