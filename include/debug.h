#pragma once

#include <kio.h>
#include <stdint.h>

#define _STRINGIFY(x) #x
#define _TOSTR(x)     _STRINGIFY(x)

// Exit QEMU via AArch64 semihosting SYS_EXIT.
[[noreturn]] static inline void _khcf(void) {
    volatile uint64_t block[2] = {
        0x20026ULL,  // ADP_Stopped_ApplicationExit
        1ULL         // exit code 1 — abnormal termination
    };
    __asm__ volatile (
        "mov x0, #0x18\n\t"
        "mov x1, %0\n\t"
        "hlt #0xf000"
        :
        : "r"((volatile uint64_t *)block)
        : "x0", "x1", "memory"
    );
    __builtin_unreachable();
}

// KPANIC(msg) — print a message and halt.
#define KPANIC(msg)                                          \
    do {                                                     \
        kputs("PANIC: " msg "\n");                           \
        kputs("  at " __FILE__ ":" _TOSTR(__LINE__) "\n");  \
        _khcf();                                             \
    } while (0)

// ASSERT(cond) — halt with a message if cond is false.
#define ASSERT(cond)                                             \
    do {                                                         \
        if (!(cond)) {                                           \
            kputs("ASSERT failed: " #cond "\n");                 \
            kputs("  at " __FILE__ ":" _TOSTR(__LINE__) "\n");  \
            _khcf();                                             \
        }                                                        \
    } while (0)

// ASSERT_MSG(cond, msg) — like ASSERT but with an extra message.
#define ASSERT_MSG(cond, msg)                                        \
    do {                                                             \
        if (!(cond)) {                                               \
            kputs("ASSERT failed: " #cond " — " msg "\n");          \
            kputs("  at " __FILE__ ":" _TOSTR(__LINE__) "\n");      \
            _khcf();                                                 \
        }                                                            \
    } while (0)

// UNREACHABLE() — marks code that must never execute.
#define UNREACHABLE()   KPANIC("unreachable code reached")

// BUG_ON(cond) — like ASSERT but reads as "halt if this bad thing is true".
#define BUG_ON(cond)    ASSERT(!(cond))

// Compile-time assertion.
#define STATIC_ASSERT(cond, msg)  static_assert(cond, msg)
