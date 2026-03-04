#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <cinux/terminal.h>
#include <cinux/framebuffer.h>
#include <cinux/font.h>
#include <cinux/uptime.h>

#define FONT_W      8
#define FONT_H      16
#define FG_COLOR    0x00AAAAAA
#define BG_COLOR    0x00000000

static uint32_t term_cols;
static uint32_t term_rows;
static uint32_t cursor_x;
static uint32_t cursor_y;

static void fb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    uint32_t *fb = (uint32_t *)g_fb.base;
    fb[y * (g_fb.pitch / 4) + x] = color;
}

void draw_char(uint32_t col, uint32_t row, unsigned char c) {
    uint32_t px = col * FONT_W;
    uint32_t py = row * FONT_H;

    for (uint32_t y = 0; y < FONT_H; y++) {
        uint8_t bits = font8x16[(uint8_t)c][y];
        for (uint32_t x = 0; x < FONT_W; x++) {
            fb_put_pixel(px + x, py + y,
                (bits & (0x80 >> x)) ? FG_COLOR : BG_COLOR);
        }
    }
}

static void scroll_up(void) {
    uint32_t *fb = (uint32_t *)g_fb.base;
    uint32_t row_pixels = g_fb.pitch / 4;
    uint32_t copy_rows  = (term_rows - 1) * FONT_H;

    for (uint32_t y = 0; y < copy_rows; y++) {
        for (uint32_t x = 0; x < g_fb.width; x++) {
            fb[y * row_pixels + x] = fb[(y + FONT_H) * row_pixels + x];
        }
    }

    for (uint32_t y = copy_rows; y < g_fb.height; y++) {
        for (uint32_t x = 0; x < g_fb.width; x++) {
            fb[y * row_pixels + x] = BG_COLOR;
        }
    }
}

static void newline(void) {
    cursor_x = 0;
    if (cursor_y + 1 >= term_rows) {
        scroll_up();
    } else {
        cursor_y++;
    }
}

static void put_char(char c) {
    if (c == '\n') {
        newline();
        return;
    }
    if (c == '\r') {
        cursor_x = 0;
        return;
    }
    if (c == '\t') {
        uint32_t next = (cursor_x + 8) & ~7u;
        while (cursor_x < next && cursor_x < term_cols) {
            draw_char(cursor_x++, cursor_y, ' ');
        }
        if (cursor_x >= term_cols) newline();
        return;
    }
    if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            draw_char(cursor_x, cursor_y, ' ');
        }
        return;
    }

    draw_char(cursor_x, cursor_y, (unsigned char)c);
    cursor_x++;
    if (cursor_x >= term_cols) {
        newline();
    }
}

static void print_str(const char *s) {
    while (*s) put_char(*s++);
}

static void print_uint(uint64_t val, uint32_t base, int width, char pad, int uppercase) {
    static const char *digits_lower = "0123456789abcdef";
    static const char *digits_upper = "0123456789ABCDEF";
    const char *digits = uppercase ? digits_upper : digits_lower;
    char buf[64];
    int i = 0;

    if (val == 0) {
        buf[i++] = '0';
    } else {
        while (val > 0) {
            buf[i++] = digits[val % base];
            val /= base;
        }
    }

    while (i < width) buf[i++] = pad;

    for (int j = i - 1; j >= 0; j--) put_char(buf[j]);
}

static void print_int(int64_t val, int width, char pad) {
    if (val < 0) {
        put_char('-');
        print_uint((uint64_t)(-val), 10, width - 1, pad, 0);
    } else {
        print_uint((uint64_t)val, 10, width, pad, 0);
    }
}

void terminal_init(void) {
    term_cols = g_fb.width  / FONT_W;
    term_rows = g_fb.height / FONT_H;
    cursor_x  = 0;
    cursor_y  = 0;

    uint32_t *fb = (uint32_t *)g_fb.base;
    uint32_t total = (g_fb.pitch / 4) * g_fb.height;
    for (uint32_t i = 0; i < total; i++) fb[i] = BG_COLOR;
}

void vprintk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt != '%') {
            put_char(*fmt++);
            continue;
        }
        fmt++;

        char pad = ' ';
        int  width = 0;
        int  lng = 0;

        if (*fmt == '0') { pad = '0'; fmt++; }
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }
        if (*fmt == 'l') { lng++; fmt++; }
        if (*fmt == 'l') { lng++; fmt++; }

        switch (*fmt) {
        case 'd': case 'i': {
            int64_t val = (lng >= 2) ? va_arg(args, long long)
                        : (lng == 1) ? va_arg(args, long)
                        : va_arg(args, int);
            print_int(val, width, pad);
            break;
        }
        case 'u': {
            uint64_t val = (lng >= 2) ? va_arg(args, unsigned long long)
                         : (lng == 1) ? va_arg(args, unsigned long)
                         : va_arg(args, unsigned int);
            print_uint(val, 10, width, pad, 0);
            break;
        }
        case 'x': {
            uint64_t val = (lng >= 2) ? va_arg(args, unsigned long long)
                         : (lng == 1) ? va_arg(args, unsigned long)
                         : va_arg(args, unsigned int);
            print_uint(val, 16, width, pad, 0);
            break;
        }
        case 'X': {
            uint64_t val = (lng >= 2) ? va_arg(args, unsigned long long)
                         : (lng == 1) ? va_arg(args, unsigned long)
                         : va_arg(args, unsigned int);
            print_uint(val, 16, width, pad, 1);
            break;
        }
        case 'o': {
            uint64_t val = (lng >= 2) ? va_arg(args, unsigned long long)
                         : (lng == 1) ? va_arg(args, unsigned long)
                         : va_arg(args, unsigned int);
            print_uint(val, 8, width, pad, 0);
            break;
        }
        case 'p': {
            uint64_t val = (uint64_t)va_arg(args, void *);
            print_str("0x");
            print_uint(val, 16, 16, '0', 0);
            break;
        }
        case 'c':
            put_char((char)va_arg(args, int));
            break;
        case 's': {
            const char *s = va_arg(args, const char *);
            if (!s) s = "(null)";
            int len = 0;
            const char *tmp = s;
            while (*tmp++) len++;
            int pad_len = width - len;
            while (pad_len-- > 0) put_char(pad);
            print_str(s);
            break;
        }
        case '%':
            put_char('%');
            break;
        default:
            put_char('%');
            put_char(*fmt);
            break;
        }
        fmt++;
    }

    va_end(args);
}

void printk(const char *fmt, ...) {
    uint64_t us = uptime_us();
    uint64_t secs = us / 1000000ULL;
    uint64_t frac = us % 1000000ULL;

    int digits = 1;
    uint64_t tmp = secs;
    while (tmp >= 10) { tmp /= 10; digits++; }
    int spaces = 5 - digits;

    put_char('[');
    for (int i = 0; i < spaces; i++) put_char(' ');
    print_uint(secs, 10, 0, ' ', 0);
    put_char('.');
    print_uint(frac, 10, 6, '0', 0);
    put_char(']');
    put_char(' ');

    va_list args;
    va_start(args, fmt);
    vprintk(fmt, args);
    va_end(args);
}