#ifndef MEMORY_H
#define MEMORY_H

#include <common.h>

uint32_t kmalloc(size_t size);
uint32_t kmalloc_a(size_t size);
uint32_t kmalloc_p(size_t size, uint32_t* phys);
uint32_t kmalloc_ap(size_t size, uint32_t* phys);

void* memmove(void* dstptr, const void* srcptr, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
void* memcpy(void* dstptr, const void* srcptr, size_t size);

#endif