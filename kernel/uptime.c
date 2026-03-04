#include <cinux/tsc.h>
#include <cinux/pit.h>

static uint64_t tsc_boot = 0;
static uint64_t tsc_frequency = 0;

uint64_t measure_tsc_freq(void) {
    uint64_t t0 = read_tsc();
    pit_usleep(10000);
    uint64_t t1 = read_tsc();
    uint64_t delta = t1 - t0;
    return delta * 100;
}

void tsc_init(void) {
    tsc_frequency = measure_tsc_freq();
    tsc_boot = read_tsc();
}

uint64_t uptime_us(void) {
    uint64_t now = read_tsc();
    uint64_t delta = now - tsc_boot;
    return (delta * 1000000ULL) / tsc_frequency;
}
