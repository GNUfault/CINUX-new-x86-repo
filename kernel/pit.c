#include <cinux/asm.h>
#include <cinux/io.h>

#define PIT_FREQUENCY 1193182
#define PIT_DIVISOR   119

volatile uint64_t pit_ticks = 0;

void pit_init(void) {
    uint16_t divisor = PIT_DIVISOR;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void pit_irq_handler(void) {
    pit_ticks++;
}

void pit_usleep(uint64_t us) {
    uint64_t ticks_needed = us / 100;
    uint64_t start = pit_ticks;
    while ((pit_ticks - start) < ticks_needed) {
        hlt();
    }
}