#ifndef GDT_FLUSH_H
#define GDT_FLUSH_H

#include <stdint.h>

extern void gdt_flush(uint64_t gdt_ptr);
extern void tss_flush(void);

#endif // GDT_FLUSH_H