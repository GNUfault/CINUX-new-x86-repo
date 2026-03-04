#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint64_t base;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
} framebuffer_t;

extern framebuffer_t g_fb;

#endif // FRAMEBUFFER_H