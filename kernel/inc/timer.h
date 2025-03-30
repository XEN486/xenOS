#ifndef TIMER_H
#define TIMER_H

#include <common.h>

// Initializes the PIT at a specific frequency (in Hz).
void timer_init(uint32_t freq);

#endif