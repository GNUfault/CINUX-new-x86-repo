#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>
#include <cinux/framebuffer.h>

extern void kmain(framebuffer_t *fb_info);

#endif // BOOT_H