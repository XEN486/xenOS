#include <timer.h>
#include <idt.h>

uint32_t tick = 0;

static void timer_callback(int_registers_t* regs) {
    tick++;
}

void timer_init(uint32_t freq) {
    // register the timer callback.
    register_int_handler(IRQ0, timer_callback);

    // find frequency divisor.
    uint32_t divisor = 1193180 / freq;

    // send command byte to the PIT.
    outb(0x43, 0x36);

    // send frequency divisor byte-wise to the PIT.
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}