#include <stdint.h>
#include <cinux/idt.h>
#include <cinux/pit.h>
#include <cinux/io.h>
#include <cinux/asm.h>
#include <cinux/idt_asm.h>

#define IDT_ENTRIES  256
#define KERNEL_CS    0x38
#define IDT_GATE_INT 0x8E

#define PIC1_CMD     0x20
#define PIC1_DATA    0x21
#define PIC2_CMD     0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

#define ICW1_INIT    0x11
#define ICW4_8086    0x01

extern void isr_default(void);
extern void isr_pit(void);

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t   idt_ptr;

static void pic_init(uint8_t offset1, uint8_t offset2) {
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    outb(PIC1_CMD,  ICW1_INIT);
    outb(PIC2_CMD,  ICW1_INIT);
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void idt_set_gate(uint8_t vector, uint64_t handler, uint8_t type_attr) {
    idt[vector].offset_low  = (uint16_t)(handler & 0xFFFF);
    idt[vector].selector    = KERNEL_CS;
    idt[vector].ist         = 0;
    idt[vector].type_attr   = type_attr;
    idt[vector].offset_mid  = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[vector].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    idt[vector].zero        = 0;
}

void idt_init(void) {
    idt_ptr.limit = (uint16_t)(sizeof(idt) - 1);
    idt_ptr.base  = (uint64_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate((uint8_t)i, (uint64_t)isr_default, IDT_GATE_INT);
    }

    idt_set_gate(0x20, (uint64_t)isr_pit, IDT_GATE_INT);

    lidt_load(&idt_ptr);

    pic_init(0x20, 0x28);
    outb(PIC1_DATA, 0xFE);
    outb(PIC2_DATA, 0xFF);

    pit_init();

    sti();
}

void irq_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

void isr_pit_handler(void) {
    pit_irq_handler();
    irq_eoi(0);
}