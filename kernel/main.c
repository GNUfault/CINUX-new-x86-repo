#include <cinux/idt.h>
#include <cinux/pit.h>
#include <cinux/uptime.h>
#include <cinux/paging.h>

void kmain(void) {
    idt_init();
    for(;;);
    pit_init();
    tsc_init();
    paging_init();
    for(;;);
}