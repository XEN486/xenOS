#include <common.h>

void hang(bool halt) {
    if (!halt)
        while (true) {}

hlt:
    // clear interrupts and halt.
    __asm__ volatile("cli; hlt");

    // repeat again if we wake up somehow.
    goto hlt;
}