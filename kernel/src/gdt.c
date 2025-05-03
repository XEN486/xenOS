#include <gdt.h>

extern void gdt_apply(uint32_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

// sets the value of a GDT entry.
static void gdt_set_entry(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= granularity & 0xF0;
    gdt_entries[num].access = access;
}

void gdt_init() {
    // set GDT limit and base.
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // set GDT entries.
    gdt_set_entry(0, 0x00000000, 0x00000000, 0x00, 0x00); // null segment
    gdt_set_entry(1, 0x00000000, 0xFFFFFFFF, 0x9A, 0xCF); // code segment
    gdt_set_entry(2, 0x00000000, 0xFFFFFFFF, 0x92, 0xCF); // data segment
    gdt_set_entry(3, 0x00000000, 0xFFFFFFFF, 0xFA, 0xCF); // user mode code segment
    gdt_set_entry(4, 0x00000000, 0xFFFFFFFF, 0xF2, 0xCF); // user mode data segment

    // apply the new GDT.
    gdt_apply((uint32_t)&gdt_ptr);
}