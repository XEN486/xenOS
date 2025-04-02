#include <common.h>
#include <terminal.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <paging.h>
#include <keyboard.h>
#include <kmalloc.h>

void print_key(uint8_t scancode, uint8_t ascii, uint8_t modifiers) {
	if (ascii != 0 && (modifiers & CTRL_PRESSED) == 0) {
		terminal_put(ascii);
	}
}

void kmain(unsigned long magic, unsigned long addr) {
	terminal_init();
	gdt_init();
	idt_init();
	timer_init(50);

	terminal_write("test");
	paging_init();
	terminal_write("test");
	keyboard_init();

	register_key_handler(print_key);
	while (true) {}
}