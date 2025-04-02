#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <common.h>

// modifiers & SHIFT_PRESSED
#define SHIFT_PRESSED   0b00000001

// modifiers & CAPS_LOCK
#define CAPS_LOCK       0b00000010

// modifiers & CTRL_PRESSED
#define CTRL_PRESSED    0b00000100

// Callback for keyboard events.
typedef void (*key_handler_t)(uint8_t scancode, uint8_t ascii, uint8_t modifiers);

// Initializes the keyboard.
void keyboard_init();

// Registers a keyboard callback.
void register_key_handler(key_handler_t handler);

#endif