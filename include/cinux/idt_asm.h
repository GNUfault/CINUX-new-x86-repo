#ifndef IDT_ASM_H
#define IDT_ASM_H

#include <cinux/idt.h>

void isr_default(void);
void isr_pit(void);
void lidt_load(idt_ptr_t *ptr);

#endif // IDT_ASM_H