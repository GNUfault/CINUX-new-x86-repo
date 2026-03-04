#include <cinux/uptime.h>
#include <cinux/paging.h>

void kmain(void) {
    tsc_init();
    paging_init();
    for(;;);
}