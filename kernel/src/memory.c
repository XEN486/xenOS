#include <memory.h>
#include <common.h>
#include <heap.h>
#include <paging.h>
#include <terminal.h>

extern uint32_t end;
extern page_directory_t* kernel_directory;
extern heap_t* kernel_heap;

uint32_t placement_address = (uint32_t)&end;

void kfree(void* p) {
    heap_free(p, kernel_heap);
}

uint32_t kmalloc_i(size_t size, bool align, uint32_t* phys) {
    // if the kernel heap is defined, allocate using the heap.
    if (kernel_heap != 0) {
        // allocate memory.
        void* addr = heap_alloc(size, align, kernel_heap);

        // get the physical address of the memory.
        if (phys != NULL) {
            page_t* page = paging_get_page((uint32_t)addr, false, kernel_directory);
            *phys = (page->frame * 0x1000) + ((uint32_t)addr & 0xFFF);
        }

        // return the address.
        return (uint32_t)addr;
    }

    // if the kernel heap is not defined, allocate using placement address.
    else {
        // page align the placement address if necessary.
        if (align == true && (placement_address & 0x00000FFF)) {
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
    
        // set the physical address.
        if (phys) {
            *phys = placement_address;
        }
    
        // increment the placement address.
        uint32_t tmp = placement_address;
        placement_address += size;
        return tmp;
    }
}

uint32_t kmalloc(size_t size) {
    return kmalloc_i(size, false, NULL);

}
uint32_t kmalloc_a(size_t size) {
    return kmalloc_i(size, true, NULL);
}

uint32_t kmalloc_p(size_t size, uint32_t* phys) {
    return kmalloc_i(size, false, phys);
}

uint32_t kmalloc_ap(size_t size, uint32_t* phys) {
    return kmalloc_i(size, true, phys);
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
    uint8_t* dst = (uint8_t*) dstptr;
    const uint8_t* src = (const uint8_t*) srcptr;

    if (dst < src) {
        for (size_t i = 0; i < size; i++)
            dst[i] = src[i];
    } else {
        for (size_t i = size; i != 0; i--)
            dst[i-1] = src[i-1];
    }

    return dstptr;
}

int memcmp(const void* aptr, const void* bptr, size_t size) {
    const uint8_t* a = (const uint8_t*) aptr;
    const uint8_t* b = (const uint8_t*) bptr;

    for (size_t i = 0; i < size; i++) {
        if (a[i] < b[i])
            return -1;
        else if (b[i] < a[i])
            return 1;
    }

    return 0;
}

void* memset(void* bufptr, int value, size_t size) {
    uint8_t* buf = (uint8_t*) bufptr;
    
    for (size_t i = 0; i < size; i++)
        buf[i] = (uint8_t) value;

    return bufptr;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
    uint8_t* dst = (uint8_t*) dstptr;
    const uint8_t* src = (const uint8_t*) srcptr;

    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];

    return dstptr;
}