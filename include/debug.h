#pragma once

#include <kio.h>

#define _STRINGIFY(x) #x
#define _TOSTR(x)     _STRINGIFY(x)

// Halt and catch fire — used by panic/assert.
[[noreturn]] static inline void _khcf(void) {
    for (;;) {
        __asm__ volatile("wfi");
    }
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
