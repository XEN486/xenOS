#ifndef PANIC_H
#define PANIC_H

#include <common.h>

#define PANIC(error, description, handler) panic(error, description, __FILE__, __LINE__, handler)

// Callback for kernel panics or notifications.
typedef void (*panic_handler_t)();

// Raise a kernel panic.
void panic(const char* error, const char* description, const char* file, int line, panic_handler_t handler);

// Notify the user about a problem.
void notify(const char* error, const char* description, panic_handler_t handler);

// Notify if false.
void assert(bool condition, const char* error, const char* description);

#endif