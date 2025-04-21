#include <multiboot2.h>
#include <common.h>
#include <terminal.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <paging.h>
#include <keyboard.h>

static void print_key(uint8_t scancode, uint8_t ascii, uint8_t modifiers) {
	//terminal_write_hex(scancode);
	if (ascii != 0 && (modifiers & CTRL_PRESSED) == 0) {
		terminal_put(ascii);
	}

	(void)scancode;
}

static void check_magic(unsigned long magic) {
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		terminal_write("invalid magic number: ");
		terminal_write_hex(magic);
		hang(true);
	}
}

void kmain(unsigned long magic, unsigned long addr) {
	terminal_init();
	check_magic(magic);

	gdt_init();
	idt_init();
	timer_init(50);
	paging_init();

	keyboard_init();
	register_key_handler(print_key);

	hang(false);
	(void)addr;
}