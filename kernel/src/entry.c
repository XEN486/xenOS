#include <common.h>
#include <terminal.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <paging.h>
#include <keyboard.h>
#include <panic.h>
#include <memory.h>

void print_key(uint8_t scancode, uint8_t ascii, uint8_t modifiers) {
	//terminal_write_hex(scancode);
	if (ascii != 0 && (modifiers & CTRL_PRESSED) == 0) {
		terminal_put(ascii);
	}
}

void kmain(unsigned long magic, unsigned long addr) {
	terminal_init();
	gdt_init();
	idt_init();
	timer_init(50);

	uint32_t a = kmalloc(8);
	paging_init();
	uint32_t b = kmalloc(8);
	uint32_t c = kmalloc(8);

	//keyboard_init();
	//register_key_handler(print_key);

	terminal_write("a: ");
	terminal_write_hex(a);

	terminal_write(", b: ");
	terminal_write_hex(b);

	terminal_write("\nc: ");
	terminal_write_hex(c);

	kfree(c);
	kfree(b);

	uint32_t d = kmalloc(12);
	terminal_write(", d: ");
	terminal_write_hex(d);

	//uint32_t* unmapped = (uint32_t*)0xdeadbeef;
	//terminal_put(*unmapped);

	while (true) {}
}