#ifndef HEAP_H
#define HEAP_H

#include <common.h>
#include <ordered_array.h>

#define HEAP_START 0xC0000000
#define HEAP_INITIAL_SIZE 0x100000
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x45485041
#define HEAP_MIN_SIZE 0x70000

typedef struct {
    uint32_t magic;
    bool hole;
    size_t size;
} header_t;

typedef struct {
    uint32_t magic;
    header_t* header;
} footer_t;

typedef struct {
    ordered_array_t index;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_address;
    bool supervisor;
    bool readonly;
} heap_t;

// Creates a new heap.
heap_t* heap_create(uint32_t start, uint32_t end, uint32_t max, bool supervisor, bool readonly);

// Allocates a contiguous region of memory in the heap.
void* heap_alloc(size_t size, bool page_align, heap_t* heap);

// Releases a block that was allocated with 'heap_alloc'
void heap_free(void* p, heap_t* heap);

#endif