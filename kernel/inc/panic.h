#ifndef PANIC_H
#define PANIC_H

// Callback for kernel panics.
typedef void (*panic_handler_t)();

// Raise a kernel panic.
void panic(const char* error, const char* description, panic_handler_t handler);

#endif