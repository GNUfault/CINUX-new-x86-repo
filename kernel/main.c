#include <cinux/idt.h>
#include <cinux/pit.h>
#include <cinux/uptime.h>
#include <cinux/terminal.h>
#include <cinux/framebuffer.h>
#include <cinux/font.h>
#include <cinux/asm.h>

void kmain(framebuffer_t *fb_info) {
    terminal_init();
    vprintk("CINUX is running...\n");
    idt_init();
    pit_init();
    sti();
    tsc_init();
    
    printk("test kernel message\n");   

    for(;;) hlt();
}
