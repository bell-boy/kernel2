#pragma once
#include <kstdlib/kio.h>

#define KERNEL_TEST_MAGIC "STARTING KERNEL INTEGRATION TEST"
#define TEST_MAIN(...) \
    extern "C" void main() { \
        kstd::kprintf("%s\n", KERNEL_TEST_MAGIC); \
        __VA_ARGS__ \
    }
