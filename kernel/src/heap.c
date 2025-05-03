#include <heap.h>
#include <memory.h>
#include <panic.h>
#include <paging.h>

extern page_directory_t* kernel_directory;
heap_t* kernel_heap = 0;

static int32_t find_smallest_hole(size_t size, bool page_align, heap_t* heap) {
    size_t iterator = 0;
    while (iterator < heap->index.size) {
        header_t* header = (header_t*)lookup_ordered_array(iterator, &heap->index);

        // check if page alignment is needed.
        if (page_align == true) {
            uint32_t location = (uint32_t)header;
            int32_t offset = 0;

            if (((location + sizeof(header_t)) & 0xFFFFF000) != 0)
                offset = 0x1000 - (location+sizeof(header_t)) % 0x1000;

            // check if adjusted hole is big enough.
            int32_t hole_size = (int32_t)header->size - offset;
            if (hole_size >= (int32_t)size)
                break;
        }

        // check if hole is big enough.
        else if (header->size >= size)
            break;

        iterator++;
    }

    // return -1 if no suitable hole was found.
    if (iterator == heap->index.size)
        return -1;

    return iterator;
}

static bool header_t_less_than(void* a, void* b) {
    return (((header_t*)a)->size < ((header_t*)b)->size) ? true : false;
}

static void expand(size_t new_size, heap_t* heap) {
    // ensure new size is larger than current size.
    assert(new_size > heap->end_address - heap->start_address, "Heap is too small", "Tried to expand heap further than the maximum size of the heap.");

    // align new size to page boundary.
    if ((new_size & 0xFFFFF000) != 0) {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    // check if expansion exceeds max address.
    assert(heap->start_address + new_size <= heap->max_address, "Heap is too small", "Tried to expand heap further than max address.");

    // map new pages.
    size_t old_size = heap->end_address - heap->start_address;
    size_t i = old_size;
    while (i < new_size) {
        page_t* page = paging_get_page(heap->start_address + i, true, kernel_directory);
        paging_alloc_frame(page, heap->supervisor, !heap->readonly);
        i += 0x1000;
    }

    // update end address.
    heap->end_address = heap->start_address + new_size;
}

static size_t contract(uint32_t new_size, heap_t* heap) {
    // make sure new size is smaller than current.
    assert(new_size < heap->end_address - heap->start_address, "", "");

    // align down to page boundary.
    if (new_size & 0x1000) {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    // don't contract below minimum size.
    if (new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;

    // unmap pages.
    size_t old_size = heap->end_address - heap->start_address;
    size_t i = old_size - 0x1000;
    while (new_size < i) {
        page_t* page = paging_get_page(heap->start_address + i, false, kernel_directory);
        paging_free_frame(page);
        i -= 0x1000;
    }

    // update end address.
    heap->end_address = heap->start_address + new_size;
    return new_size;
}

heap_t* heap_create(uint32_t start, uint32_t end, uint32_t max, bool supervisor, bool readonly) {
    // allocate heap structure.
    heap_t* heap = (heap_t*)kmalloc(sizeof(heap_t));

    // check for page alignment.
    assert(start % 0x1000 == 0, "Heap not aligned", "The heap start address is not page aligned.");
    assert(end % 0x1000 == 0, "Heap not aligned", "The heap end address is not page aligned.");

    // set up ordered array index.
    heap->index = place_ordered_array((void*)start, HEAP_INDEX_SIZE, &header_t_less_than);
    start += sizeof(type_t) * HEAP_INDEX_SIZE;

    // align start.
    if ((start & 0xFFFFF000) != 0) {
        start &= 0xFFFFF000;
        start += 0x1000;
    }

    // set addresses and flags.
    heap->start_address = start;
    heap->end_address = end;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    // create initial hole.
    header_t* hole = (header_t*)start;
    hole->size = end - start;
    hole->magic = HEAP_MAGIC;
    hole->hole = 1;

    // insert the hole into the array.
    insert_ordered_array((void*)hole, &heap->index);

    return heap;
}

void* heap_alloc(size_t size, bool page_align, heap_t* heap) {
    // calculate total size with header and footer.
    size_t new_size = size + sizeof(header_t) + sizeof(footer_t);
    
    // find smallest fitting hole.
    int32_t iterator = find_smallest_hole(new_size, page_align, heap);

    // expand heap if no hole found.
    if (iterator == -1) {
        // store old heap size and end.
        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t old_end_address = heap->end_address;

        // increase heap size.
        expand(old_length + new_size, heap);
        uint32_t new_length = heap->end_address - heap->start_address;

        // find largest block.
        iterator = 0;
        int32_t idx = -1; uint32_t value = 0x0;
        while (iterator < (int32_t)heap->index.size) {
            uint32_t tmp = (uint32_t)lookup_ordered_array(iterator, &heap->index);
            if (tmp > value) {
                value = tmp;
                idx = iterator;
            }
            iterator++;
        }

        // create new hole if none found.
        if (idx == -1) {
            header_t* header = (header_t*)old_end_address;
            header->magic = HEAP_MAGIC;
            header->size = new_length - old_length;
            header->hole = true;

            // write footer.
            footer_t* footer = (footer_t*)(old_end_address + header->size - sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;

            // add to index.
            insert_ordered_array((void*)header, &heap->index);
        }

        // grow existing hole if found.
        else {
            header_t* header = lookup_ordered_array(idx, &heap->index);
            header->size += new_length - old_length;

            // write new footer.
            footer_t* footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }

        // retry allocation.
        return heap_alloc(size, page_align, heap);
    }

    // get original hole info.
    header_t* orig_hole_header = (header_t*)lookup_ordered_array(iterator, &heap->index);
    uint32_t orig_hole_pos = (uint32_t)orig_hole_header;
    size_t orig_hole_size = orig_hole_header->size;

    // use full hole if remaining space too small.
    if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
        size += orig_hole_size - new_size;
        new_size = orig_hole_size;
    }

    // align block if needed.
    if (page_align && orig_hole_pos & 0xFFFFF000) {
        uint32_t new_location = orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);

        // create hole before aligned block.
        header_t* hole_header = (header_t*)orig_hole_pos;
        hole_header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
        hole_header->magic = HEAP_MAGIC;
        hole_header->hole = true;

        // write footer for pre-align hole.
        footer_t* hole_footer = (footer_t*)((uint32_t)new_location - sizeof(footer_t));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole_header;

        // update position and size.
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header->size;
    }

    // remove hole from index.
    else {
        remove_ordered_array(iterator, &heap->index);
    }

    // create header for new block.
    header_t* block_header = (header_t*)orig_hole_pos;
    block_header->magic = HEAP_MAGIC;
    block_header->hole = false;
    block_header->size = new_size;

    // create footer for new block.
    footer_t* block_footer = (footer_t*)(orig_hole_pos + sizeof(header_t) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block_header;

    // if space left, create new hole after.
    if (orig_hole_size - new_size > 0) {
        header_t* hole_header = (header_t*)(orig_hole_pos + sizeof(header_t) + size + sizeof(footer_t));
        hole_header->magic = HEAP_MAGIC;
        hole_header->hole = true;
        hole_header->size = orig_hole_size - new_size;

        // write footer for new hole.
        footer_t* hole_footer = (footer_t*)((uint32_t)hole_header + orig_hole_size - new_size - sizeof(footer_t));
        if ((uint32_t)hole_footer < heap->end_address) {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }

        // insert hole into index.
        insert_ordered_array((void*)hole_header, &heap->index);
    }

    // return usable pointer.
    return (void*)((uint32_t)block_header + sizeof(header_t));
}

void heap_free(void* p, heap_t* heap) {
    // return if null.
    if (p == 0)
        return;
    
    // get header and footer.
    header_t* header = (header_t*)((uint32_t)p - sizeof(header_t));
    footer_t* footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));

    // check magic values.
    assert(header->magic == HEAP_MAGIC, "Wrong heap magic", "The heap's header magic was incorrect while freeing memory.");
    assert(footer->magic == HEAP_MAGIC, "Wrong heap magic", "The heap's footer magic was incorrect while freeing memory.");

    // mark as hole.
    header->hole = true;
    bool do_add = true;

    // check left neighbor.
    footer_t* test_footer = (footer_t*)((uint32_t)header - sizeof(footer_t));
    if (test_footer->magic == HEAP_MAGIC && test_footer->header->hole == true) {
        uint32_t cache_size = header->size;
        header = test_footer->header;

        // merge with left.
        footer->header = header;
        header->size += cache_size;

        do_add = false;
    }

    // check right neighbor.
    header_t* test_header = (header_t*)((uint32_t)footer + sizeof(footer_t));
    if (test_header->magic == HEAP_MAGIC && test_header->hole) {
        // merge with right.
        header->size += test_header->size;

        // update footer.
        test_footer = (footer_t*)((uint32_t)test_header + test_header->size - sizeof(footer_t));
        footer = test_footer;

        // remove right hole from index.
        size_t iterator = 0;
        while ((iterator < heap->index.size) && (lookup_ordered_array(iterator, &heap->index) != (void*)test_header))
            iterator++;

        assert(iterator < heap->index.size, "", "");
        remove_ordered_array(iterator, &heap->index);
    }

    // shrink heap if block at end.
    if ((uint32_t)footer + sizeof(footer_t) == heap->end_address) {
        uint32_t old_length = heap->end_address - heap->start_address;
        size_t new_length = contract((uint32_t)header - heap->start_address, heap);

        // update size if partial shrink.
        if (header->size - (old_length - new_length) > 0) {
            header->size -= old_length - new_length;
            
            footer = (footer_t*)((uint32_t)header + header->size - sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        }

        // else remove block.
        else {
            size_t iterator = 0;
            while ((iterator < heap->index.size) && (lookup_ordered_array(iterator, &heap->index) != (void*)test_header))
                iterator++;
            
            if (iterator < heap->index.size)
                remove_ordered_array(iterator, &heap->index);
        }
    }

    // add block to index if needed.
    if (do_add == true)
        insert_ordered_array((void*)header, &heap->index);
}