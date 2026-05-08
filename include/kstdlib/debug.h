#pragma once

#include <kstdlib/kio.h>
#include <stdint.h>

#define _STRINGIFY(x) #x
#define _TOSTR(x)     _STRINGIFY(x)

// KPANIC(msg) — print a message and halt.
#define KPANIC(msg)                                          \
    do {                                                     \
        kstd::kputs("PANIC: " msg "\n");                     \
        kstd::kputs("  at " __FILE__ ":" _TOSTR(__LINE__) "\n"); \
        kstd::kexit(1);                                       \
    } while (0)

// ASSERT(cond) — halt with a message if cond is false.
#define ASSERT(cond)                                             \
    do {                                                         \
        if (!(cond)) {                                           \
            kstd::kputs("ASSERT failed: " #cond "\n");           \
            kstd::kputs("  at " __FILE__ ":" _TOSTR(__LINE__) "\n"); \
            kstd::kexit(1);                                       \
        }                                                        \
    } while (0)

// ASSERT_MSG(cond, msg) — like ASSERT but with an extra message.
#define ASSERT_MSG(cond, msg)                                        \
    do {                                                             \
        if (!(cond)) {                                               \
            kstd::kputs("ASSERT failed: " #cond " — " msg "\n");    \
            kstd::kputs("  at " __FILE__ ":" _TOSTR(__LINE__) "\n"); \
            kstd::kexit(1);                                          \
        }                                                            \
    } while (0)

// UNREACHABLE() — marks code that must never execute.
#define UNREACHABLE()   KPANIC("unreachable code reached")

// BUG_ON(cond) — like ASSERT but reads as "halt if this bad thing is true".
#define BUG_ON(cond)    ASSERT(!(cond))

// Compile-time assertion.
#define STATIC_ASSERT(cond, msg)  static_assert(cond, msg)
