#include <common.h>
#include <terminal.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <paging.h>

void kmain(unsigned long magic, unsigned long addr) {
	terminal_init();
	gdt_init();
	idt_init();
	timer_init(50);
	paging_init();
	
	asm volatile ("sti");

	terminal_write("hi from paging!");
	size_t* ptr = (size_t*)0xA0000000;
	size_t page_fault = *ptr;

	while (true) {}
}