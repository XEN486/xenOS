#ifndef PANIC_H
#define PANIC_H

#include <common.h>

// Macro that calls the `panic` function. This macro is helpful so you do not always have to add in arguments.
#define PANIC(error, description, handler) panic(error, description, handler, __FILE__, __LINE__)

// Callback for kernel panics or notifications.
typedef void (*panic_handler_t)();

// Raises a kernel panic. You should use the `PANIC` macro instead of directly calling this function.
void panic(const char* error, const char* description, panic_handler_t handler, const char* file, int line);

// Notifies the user about a potential problem.
void notify(const char* error, char* description, panic_handler_t handler);

// Notifies if false.
void assert(bool condition, const char* error, char* description);

#endif