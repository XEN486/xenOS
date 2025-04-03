#include <memory.h>
#include <common.h>

extern uint32_t end;
uint32_t placement_address = (uint32_t)&end;

uint32_t kmalloc_i(size_t size, bool align, uint32_t* phys) {
    if (align == true && (placement_address & 0x00000FFF)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    if (phys) {
        *phys = placement_address;
    }

    uint32_t tmp = placement_address;
    placement_address += size;
    return tmp;
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
    unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;

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
    const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;

	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}

	return 0;
}

void* memset(void* bufptr, int value, size_t size) {
    unsigned char* buf = (unsigned char*) bufptr;
    
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;

	return bufptr;
}

void* memcpy(void* dstptr, const void* srcptr, size_t size) {
    unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;

	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];

	return dstptr;
}