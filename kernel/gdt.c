#include <cinux/gdt.h>
#include <cinux/gdt_flush.h>
#include <stdint.h>
#include <string.h>

struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr gp;
struct tss_entry tss;

static void gdt_set(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[i].base_low = base & 0xFFFF;
    gdt[i].base_mid = (base >> 16) & 0xFF;
    gdt[i].base_high = (base >> 24) & 0xFF;
    gdt[i].limit_low = limit & 0xFFFF;
    gdt[i].gran = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].access = access;
}

static void gdt_set_tss_entry(int i, uint64_t base, uint32_t limit) {
    gdt[i].limit_low = limit & 0xFFFF;
    gdt[i].base_low = base & 0xFFFF;
    gdt[i].base_mid = (base >> 16) & 0xFF;
    gdt[i].access = 0x89;
    gdt[i].gran = ((limit >> 16) & 0x0F) | 0x00;
    gdt[i].base_high = (base >> 24) & 0xFF;

    struct gdt_entry_high *high = (struct gdt_entry_high *)&gdt[i + 1];
    high->base_upper = (base >> 32) & 0xFFFFFFFF;
    high->reserved = 0;
}

void gdt_set_tss(struct tss_entry *t) {
    memset(t, 0, sizeof(struct tss_entry));
    t->iopb = sizeof(struct tss_entry);
    gdt_set_tss_entry(5, (uint64_t)t, sizeof(struct tss_entry) - 1);
}

void gdt_init(void) {
    gp.limit = sizeof(gdt) - 1;
    gp.base = (uint64_t)&gdt;

    gdt_set(0, 0, 0, 0x00, 0x00);
    gdt_set(1, 0, 0xFFFFFFFF, 0x9A, 0x20);
    gdt_set(2, 0, 0xFFFFFFFF, 0x92, 0x00);
    gdt_set(3, 0, 0xFFFFFFFF, 0xFA, 0x20);
    gdt_set(4, 0, 0xFFFFFFFF, 0xF2, 0x00);

    gdt_set_tss(&tss);

    gdt_flush((uint64_t)&gp);
    tss_flush();
}