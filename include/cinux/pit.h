#ifndef PIT_H
#define PIT_H

#include <stdint.h>

void pit_init(void);
void pit_irq_handler(void);
void pit_usleep(uint64_t us);

#endif // PIT_H