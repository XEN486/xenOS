#ifndef TERMINAL_H
#define TERMINAL_H

#include <common.h>

// Macro to create a VGA colour entry from two VGA colours.
#define VGA_COLOUR(fg, bg) fg | (bg << 4)

// VGA colour
typedef enum {
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LIGHT_GREY = 7,
	DARK_GREY = 8,
	LIGHT_BLUE = 9,
	LIGHT_GREEN = 10,
	LIGHT_CYAN = 11,
	LIGHT_RED = 12,
	LIGHT_MAGENTA = 13,
	LIGHT_BROWN = 14,
	WHITE = 15,
} vga_colour_t;

// Terminal width
static const size_t TERM_WIDTH = 80;

// Terminal height
static const size_t TERM_HEIGHT = 25;

// Initializes the terminal.
void terminal_init();

// Clears the terminal.
void terminal_clear();

// Gets the current terminal colour.
uint8_t terminal_get_colour();

// Sets the current terminal colour.
void terminal_set_colour(uint8_t colour);

// Writes a character to the terminal.
void terminal_put(uint8_t c);

// Writes a string to the terminal, with a size.
void terminal_write_size(const char* data, size_t size);

// Writes a c-style string to the terminal.
void terminal_write(const char* data);

// Writes a padded 32-bit number to the terminal in hex.
void terminal_write_hex(uint32_t data);

// Draws a box to the screen.
void terminal_box(size_t x, size_t y, size_t width, size_t height);

// Moves the cursor to a specific location.
void terminal_move(size_t x, size_t y);

// Writes a 32-bit number to the terminal in decimal.
void terminal_write_dec(uint32_t data);

#endif