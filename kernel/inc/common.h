#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Halts the processor.
// If `halt` is `true`, it runs `cli` and `hlt`, then it loops if it wakes up from an interrupt.
// Otherwise, if `halt` is `false`, it runs an infinite loop that may wake up from an interrupt.
// Usually, `halt` should be set as `true`.
void hang(bool halt);
#endif