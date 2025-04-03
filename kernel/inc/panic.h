#ifndef PANIC_H
#define PANIC_H

// Callback for kernel panics or notifications.
typedef void (*panic_handler_t)();

// Raise a kernel panic.
void panic(const char* error, const char* description, panic_handler_t handler);

// Notify the user about a problem.
void notify(const char* error, const char* description, panic_handler_t handler);

#endif