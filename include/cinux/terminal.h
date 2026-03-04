#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

void terminal_init(void);
void vprintk(const char *fmt, ...);
void printk(const char *fmt, ...);

#endif