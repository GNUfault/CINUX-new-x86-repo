#include <cinux/idt.h>
#include <cinux/pit.h>
#include <cinux/uptime.h>
#include <cinux/terminal.h>
#include <cinux/framebuffer.h>
#include <cinux/asm.h>
#include <cinux/usermode.h>
#include <cinux/gdt.h>

void kmain(framebuffer_t *fb_info) {
    terminal_init();
    vprintk("CINUX is running... Time for freedom!\n");
    gdt_init();
    idt_init();
    pit_init();
    sti();
    tsc_init();
    
    printk("test kernel message\n");

    jump_usermode();    
}
