#ifndef TERMINAL_H
#define TERMINAL_H

#include <common.h>

// Creates a VGA colour entry.
#define VGA_COLOUR(fg, bg) fg | (bg << 4)

// VGA colour enum.
typedef enum {
    BLACK = 0,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GREY,
    DARK_GREY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    LIGHT_BROWN,
    WHITE,
} vga_colour_t;

// Terminal width.
#define TERM_WIDTH 80

// Terminal height.
#define TERM_HEIGHT 25

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