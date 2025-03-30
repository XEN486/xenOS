#include <terminal.h>
#include <string.h>

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_colour;
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

static inline uint16_t vga_entry(char character, uint8_t colour) {
	return (uint16_t)character | (uint16_t)colour << 8;
}

static void terminal_update_cursor() {
	uint16_t index = terminal_row * TERM_WIDTH + terminal_column;
	
	outb(0x3D4, 14);
	outb(0x3D5, (uint8_t)(index >> 8));
	outb(0x3D4, 15);
	outb(0x3D5, (uint8_t)(index & 0xFF));
}

static void terminal_write_entry_index(char c, uint8_t colour, size_t index) {
	vga_buffer[index] = vga_entry(c, colour);
}

static void terminal_write_entry(char c, uint8_t colour, size_t x, size_t y) {
	const size_t index = y * TERM_WIDTH + x;
	terminal_write_entry_index(c, colour, index);
}

static void terminal_scroll() {
	if (terminal_row >= TERM_HEIGHT) {
		for (size_t i = 0; i < (TERM_HEIGHT - 1) * TERM_WIDTH; i++) {
			vga_buffer[i] = vga_buffer[i + TERM_WIDTH];
		}
		
		for (size_t i = (TERM_HEIGHT - 1) * TERM_WIDTH; i < TERM_WIDTH * TERM_HEIGHT; i++) {
			terminal_write_entry_index(' ', terminal_colour, i);
		}
		
		terminal_row = TERM_HEIGHT - 1;
		terminal_update_cursor();
	}
}


void terminal_clear() {
	for (size_t y = 0; y < TERM_HEIGHT; y++) {
		for (size_t x = 0; x < TERM_WIDTH; x++) {
			terminal_write_entry(' ', terminal_colour, x, y);
		}
	}
}

void terminal_set_colour(uint8_t colour) {
	terminal_colour = colour;
}

void terminal_put(char c) {
	terminal_write_entry(c, terminal_colour, terminal_column, terminal_row);
	if (terminal_column++ == TERM_WIDTH) {
		terminal_column = 0;
		if (terminal_row++ == TERM_HEIGHT) {
			terminal_row = 0;
		}
	}
	
	terminal_scroll();
	terminal_update_cursor();
}

void terminal_write_size(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_put(data[i]);
	}
}

void terminal_write(const char* data) {
	terminal_write_size(data, strlen(data));
}

void terminal_init() {
	// initialize cursor.
	terminal_row = 0;
	terminal_column = 0;
	terminal_update_cursor();
	
	// set terminal colour.
	terminal_set_colour(VGA_COLOUR(LIGHT_GREY, BLACK));
	
	// clear the screen.
	terminal_clear();
}