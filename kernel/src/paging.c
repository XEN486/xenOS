#include <paging.h>
#include <terminal.h>
#include <string.h>

uint32_t* frames;
uint32_t nframes;

// From "heap.c"
extern size_t placement_address;

// The kernel's page directory.
page_directory_t* kernel_directory = NULL;

// The current loaded page directory.
page_directory_t* current_directory = NULL;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// sets a bit in the frames bitset.
static void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);

    frames[idx] |= (0x1 << off);
}

// clears a bit in the frames bitset
static void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);

    frames[idx] &= ~(0x1 << off);
}

// tests if a bit is set in the frames bitset.
static bool test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);

    return (frames[idx] & (0x1 << off));
}

// finds the first free frame.
static uint32_t first_free_frame() {
    for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] == 0xFFFFFFFF) {
            continue;
        }

        for (uint32_t j = 0; j < 32; j++) {
            uint32_t bit = 0x1 << j;
            if (!(frames[i] & bit)) {
                return i * 4 * 8 + j;
            }
        }
    }
}

// enables paging in CR0.
static void enable_paging() {
    // read CR0.
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));

    // set the PG bit.
    cr0 |= 0x80000000;

    // write back the new CR0.
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

// allocates a frame.
void alloc_frame(page_t* page, bool kernel, bool writable) {
    // exit if the frame was already allocated.
    if (page->frame != 0) {
        return;
    }

    // find the first free frame.
    uint32_t idx = first_free_frame();

    // panic if there are no free frames.
    if (idx == (uint32_t)-1) {
        // TODO: PANIC PANIC PANIC
        terminal_write("NO FREE FRAMES");
        while (true) {}
    }

    // set the frame in the bitset and mark the page as present.
    set_frame(idx * 0x1000);
    page->present = true;

    // set user level and writable flags based on the parameter.
    page->rw = writable;
    page->user = !kernel;
    
    // set the frame index.
    page->frame = idx;
}

// de-allocates a frame.
void free_frame(page_t* page) {
    uint32_t frame;

    // check if the page has an allocated frame.
    if (!(frame = page->frame)) {
        return;
    }

    // clear the frame from the bitset and remove it from the page.
    clear_frame(frame);
    page->frame = 0x0;
}

void paging_init() {
    // size of physical memory. we need to get this from the multiboot2 memory map, but for now only assume we have 16MB.
    uint32_t mem_end_page = 0x1000000;

    // initialize frames and nframes.
    nframes = mem_end_page / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    // create the kernel's page directory.
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    // identity map from 0x0 -> end of used memory.
    size_t i = 0;
    while (i < placement_address) {
        // get the page.
        page_t* page = paging_get_page(i, true, kernel_directory);

        // allocate the frame to be readable but not writable by userspace.
        alloc_frame(page, false, false);

        // go to next page.
        i += 0x1000;
    }

    // register the page fault handler.
    register_int_handler(14, paging_page_fault);

    // enable paging!
    paging_switch_directory(kernel_directory);
    enable_paging();
}

void paging_switch_directory(page_directory_t* new_directory) {
    // set the current directory to the new directory.
    current_directory = new_directory;

    // load the new directory into CR3.
    asm volatile("mov %0, %%cr3" :: "r"(&new_directory->physical_tables));
}

page_t* paging_get_page(uint32_t address, bool create, page_directory_t* directory) {
    // convert the address to an index.
    address /= 0x1000;

    // find the page table containing the address.
    uint32_t table_idx = address / 1024;
    if (directory->tables[table_idx]) {
        return &directory->tables[table_idx]->pages[address % 1024]; // & 1023 ?
    }
    else if (create) {
        uint32_t tmp;
        
        // allocate memory for the new page table, tmp -> physical address
        directory->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);

        // zero out the page table
        memset(directory->tables[table_idx], 0, 0x1000);

        // write the physical address to the physical_tables array with the flags "PRESENT, RW and US"
        directory->physical_tables[table_idx] = tmp | 0x7;

        // return the page.
        return &directory->tables[table_idx]->pages[address % 1024];
    }
    else {
        return 0;
    }
}

void paging_page_fault(int_registers_t* regs) {
    terminal_write("PAGE FAULT PAGE FAULT PAGE FAULT!!!!!!!!!!");
    while (true) {}
}