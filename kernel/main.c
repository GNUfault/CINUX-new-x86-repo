#include <cinux/idt.h>
#include <cinux/pit.h>
#include <cinux/uptime.h>
#include <cinux/terminal.h>
#include <cinux/framebuffer.h>
#include <cinux/font.h>
#include <cinux/asm.h>

void kmain(framebuffer_t *fb_info) {
    idt_init();
    pit_init();
    sti();
    tsc_init();
    terminal_init();
    printk("cinux booted\n");

    for(;;) hlt();
}
