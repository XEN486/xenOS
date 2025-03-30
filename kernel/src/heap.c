#include <heap.h>

extern size_t end;
size_t placement_address = (size_t)&end;

size_t kmalloc_i(size_t size, bool align, size_t* phys) {
    if (align == true && (placement_address & 0x00000FFF)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    if (phys) {
        *phys = placement_address;
    }

    size_t tmp = placement_address;
    placement_address += size;

    return tmp;
}

size_t kmalloc_a(size_t size) {
    kmalloc_i(size, true, NULL);
}

size_t kmalloc_p(size_t size, size_t* phys) {
    kmalloc_i(size, false, phys);
}

size_t kmalloc_ap(size_t size, size_t* phys) {
    kmalloc_i(size, true, phys);
}

size_t kmalloc(size_t size) {
    kmalloc_i(size, false, NULL);
}