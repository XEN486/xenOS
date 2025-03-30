#ifndef HEAP_H
#define HEAP_H

#include <common.h>

// Allocates memory at a page-aligned memory address.
size_t kmalloc_a(size_t size);

// Allocates memory, and sets the physical address of it.
size_t kmalloc_p(size_t size, size_t* phys);

// Allocates memory at a page aligned memory address, and sets the physical address of it.
size_t kmalloc_ap(size_t size, size_t* phys);

// Allocates memory.
size_t kmalloc(size_t size);

#endif