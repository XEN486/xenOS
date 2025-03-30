#ifndef STRING_H
#define STRING_H

#include <common.h>

size_t strlen(const char* str);
void* memmove(void* dstptr, const void* srcptr, size_t size);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
void* memcpy(void* dstptr, const void* srcptr, size_t size);

#endif