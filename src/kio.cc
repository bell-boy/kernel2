#include <kio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

[[noreturn]] void kexit(int code) {
    volatile uint64_t block[2] = {
        0x20026ULL,               // ADP_Stopped_ApplicationExit
        (uint64_t)(unsigned)code
    };
    asm volatile (
        "mov x0, #0x18\n\t"      // SYS_EXIT
        "mov x1, %0\n\t"
        "hlt #0xf000"
        :
        : "r"((volatile uint64_t *)block)
        : "x0", "x1", "memory"
    );
    __builtin_unreachable();
}

void kputs(const char *str) {
    // AArch64 semihosting: x0 = SYS_WRITE0 (0x04), x1 = str
    asm volatile (
        "mov x0, #0x04\n\t"
        "mov x1, %0\n\t"
        "hlt #0xf000"
        :
        : "r"(str)
        : "x0", "x1", "memory"
    );
}

static void kputs_char(char c) {
    char buf[2] = {c, '\0'};
    kputs(buf);
}

static void kputs_uint(unsigned long long n, int base) {
    const char digits[] = "0123456789abcdef";
    char buf[20];
    int i = 0;
    if (n == 0) { kputs_char('0'); return; }
    while (n > 0) {
        buf[i++] = digits[n % base];
        n /= base;
    }
    while (i-- > 0) kputs_char(buf[i]);
}

static void kputs_int(long long n) {
    if (n < 0) { kputs_char('-'); n = -n; }
    kputs_uint((unsigned long long)n, 10);
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char* p = fmt; *p; p++) {
        if (*p != '%') { kputs_char(*p); continue; }
        p++;
        switch (*p) {
            case 's': kputs(va_arg(args, const char*)); break;
            case 'd': kputs_int(va_arg(args, int));     break;
            case 'u': kputs_uint(va_arg(args, unsigned int), 10); break;
            case 'x': kputs_uint(va_arg(args, unsigned int), 16); break;
            case 'p': kputs_uint((uintptr_t)va_arg(args, void*), 16); break;
            case 'c': kputs_char((char)va_arg(args, int));        break;
            case '%': kputs_char('%'); break;
            default:  kputs_char('%'); kputs_char(*p);  break;
        }
    }

    va_end(args);
}
