#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

void tsc_init(void);
uint64_t tsc_freq(void);
uint64_t uptime_us(void);

#endif // UPTIME_H