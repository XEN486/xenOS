#include <heap.h>
#include <memory.h>
#include <panic.h>

static int32_t find_smallest_hole(size_t size, bool page_align, heap_t* heap) {
    size_t iterator = 0;
    while (iterator < heap->index.size) {
        header_t* header = (header_t*)lookup_ordered_array(iterator, &heap->index);

        if (page_align == true) {
            uint32_t location = (uint32_t)header;
            int32_t offset = 0;

            if ((location+sizeof(header_t) & 0xFFFFF000) != 0)
                offset = 0x1000 - (location+sizeof(header_t)) % 0x1000;

            int32_t hole_size = (int32_t)header->size - offset;
            if (hole_size >= (int32_t)size)
                break;
        }

        else if (header->size >= size)
            break;
    }

    if (iterator == heap->index.size)
        return -1;
    
    return iterator;
}

static bool header_t_lessthan(void* a, void* b) {
    return (((header_t*)a)->size < ((header_t*)b)->size) ? true : false;
}

static void expand(size_t new_size, heap_t* heap) {
    if (new_size & 0xFFFFF000 != 0) {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    if (heap->start_address + new_size <= heap->max_address) {
        // TODO: notify instead of panic
        panic("Heap is too small", "Tried to expand heap further than max address.", NULL);
    }
}

heap_t* heap_create(uint32_t start, uint32_t end, uint32_t max, bool supervisor, bool readonly) {
    heap_t* heap = (heap_t*)kmalloc(sizeof(heap_t));

    heap->index = place_ordered_array((void*)start, HEAP_INDEX_SIZE, &header_t_lessthan);
    start += sizeof(type_t) * HEAP_INDEX_SIZE;

    if (start & 0xFFFFF000 != 0) {
        start &= 0xFFFFF000;
        start += 0x1000;
    }

    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    header_t* hole = (header_t*)start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->hole = 1;
    insert_ordered_array((void*)hole, &heap->index);

    return heap;
}