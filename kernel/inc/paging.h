#ifndef PAGING_H
#define PAGING_H

#include <common.h>
#include <heap.h>
#include <idt.h>

typedef struct {
    uint32_t present    : 1;    // page present in memory
    uint32_t rw         : 1;    // both readable and writable
    uint32_t user       : 1;    // user mode level
    uint32_t accessed   : 1;    // page has been accessed since last refresh
    uint32_t dirty      : 1;    // page has been written to since last refresh
    uint32_t unused     : 7;    // unused/reserved bits
    uint32_t frame      : 20;   // frame address (shifted right 12 bits)
} page_t;

typedef struct {
    page_t pages[1024];
} page_table_t;

typedef struct {
    page_table_t* tables[1024];
    uint32_t physical_tables[1024];
    uint32_t physical_address;
} page_directory_t;

// Initializes paging.
void paging_init();

// Loads the specified page directory into CR3.
void paging_switch_directory(page_directory_t* new_directory);

// Retrieves a pointer to the page. If the "create" boolean is true, and the page table isn't created, create it.
page_t* paging_get_page(uint32_t address, bool create, page_directory_t* directory);

// Handles a page fault.
void paging_page_fault(int_registers_t* regs);

#endif