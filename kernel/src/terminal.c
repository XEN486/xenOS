#include <terminal.h>
#include <ports.h>
#include <memory.h>
#include <string.h>

#define TL_CORNER  0xC9  // ╔
#define TR_CORNER  0xBB  // ╗
#define BL_CORNER  0xC8  // ╚
#define BR_CORNER  0xBC  // ╝
#define HORIZONTAL 0xCD  // ═
#define VERTICAL   0xBA  // ║

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_colour;
volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

static inline uint16_t vga_entry(uint8_t character, uint8_t colour) {
    return (uint16_t)character | ((uint16_t)colour << 8);
}

static void terminal_update_cursor() {
    uint16_t index = terminal_row * TERM_WIDTH + terminal_column;
    
    outb(0x3D4, 14);
    outb(0x3D5, (uint8_t)(index >> 8));
    outb(0x3D4, 15);
    outb(0x3D5, (uint8_t)(index & 0xFF));
}

static void terminal_write_entry_index(uint8_t c, uint8_t colour, size_t index) {
    vga_buffer[index] = vga_entry(c, colour);
}

static void terminal_write_entry(uint8_t c, uint8_t colour, size_t x, size_t y) {
    const size_t index = y * TERM_WIDTH + x;
    terminal_write_entry_index(c, colour, index);
}

static void terminal_scroll() {
    if (terminal_row >= TERM_HEIGHT) {
        memmove((void*)vga_buffer, (void*)(vga_buffer + TERM_WIDTH), (TERM_HEIGHT - 1) * TERM_WIDTH * sizeof(uint16_t));

        for (size_t i = (TERM_HEIGHT - 1) * TERM_WIDTH; i < TERM_WIDTH * TERM_HEIGHT; i++) {
            terminal_write_entry_index(' ', terminal_colour, i);
        }

        terminal_row = TERM_HEIGHT - 1;
        terminal_update_cursor();
    }
}

void terminal_clear() {
    for (size_t i = 0; i < TERM_WIDTH * TERM_HEIGHT; i++) {
        terminal_write_entry_index(' ', terminal_colour, i);
    }

	terminal_column = 0;
	terminal_row = 0;
}

uint8_t terminal_get_colour() {
    return terminal_colour;
}

void terminal_set_colour(uint8_t colour) {
    terminal_colour = colour;
}

bool terminal_handle_control(uint8_t c) {
	if (c == '\n') {
		terminal_column = 0;
		terminal_row += 1;
	} else if (c == '\b') {
		if (terminal_column == 0) {
			return true;
		}

		terminal_column -= 1;
		terminal_write_entry(' ', terminal_colour, terminal_column, terminal_row);
	} else if (c == '\r') {
		terminal_column = 0;
	} else if (c == '\t') {
		terminal_column = (terminal_column + 4) & ~(4-1);
	} else {
		return false;
	}

	return true;
}

void terminal_put(uint8_t c) {
    if (!terminal_handle_control(c)) {
		terminal_write_entry(c, terminal_colour, terminal_column, terminal_row);

		if (++terminal_column == TERM_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == TERM_HEIGHT) {
				terminal_scroll();
			}
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

void terminal_write_hex(uint32_t data) {
    char hex_str[9];
	terminal_write("0x");

    for (int i = 7; i >= 0; i--) {
        hex_str[i] = "0123456789ABCDEF"[data & 0xF];
        data >>= 4;
    }
    hex_str[8] = '\0';

    terminal_write(hex_str);
}

void terminal_init() {
    terminal_set_colour(VGA_COLOUR(LIGHT_GREY, BLACK));
    terminal_clear();
    terminal_update_cursor();
}

void terminal_box(size_t x, size_t y, size_t width, size_t height) {
    terminal_move(x, y);
    
    terminal_put(TL_CORNER);
    for (size_t i = 0; i < width - 2; i++) {
        terminal_put(HORIZONTAL);
    }
    terminal_put(TR_CORNER);

    for (size_t i = 0; i < height; i++) {
        terminal_put(VERTICAL);
        for (size_t i = 0; i < width - 2; i++) {
            terminal_put(' ');
        }
        terminal_put(VERTICAL);
    }

    terminal_put(BL_CORNER);
    for (size_t i = 0; i < width - 2; i++) {
        terminal_put(HORIZONTAL);
    }
    terminal_put(BR_CORNER);
}

void terminal_move(size_t x, size_t y) {
    terminal_column = x;
    terminal_row = y;
    terminal_update_cursor();
}

void terminal_write_dec(uint32_t data) {
    char buf[11];
    char *p = &buf[sizeof(buf) - 1];
    *p = '\0';

    uint32_t value = data;

    do {
        *(--p) = "0123456789"[value % 10];
        value = value / 10;
    } while (value);

    terminal_write(p);
}