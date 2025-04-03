#include <common.h>
#include <terminal.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <paging.h>
#include <keyboard.h>
#include <panic.h>

void print_key(uint8_t scancode, uint8_t ascii, uint8_t modifiers) {
	if (ascii != 0 && (modifiers & CTRL_PRESSED) == 0) {
		terminal_put(ascii);
	}
}

void test_handler() {
	terminal_write("Hello this is from my notification handler\n");
}

void kmain(unsigned long magic, unsigned long addr) {
	terminal_init();
	gdt_init();
	idt_init();
	timer_init(50);
	notify("Test Notification", "This is a test of the new notification system.", test_handler);

	paging_init();
	keyboard_init();

	//uint32_t* unmapped = (uint32_t*)0xdeadbeef;
	//terminal_put(*unmapped);

	register_key_handler(print_key);
	while (true) {}
}