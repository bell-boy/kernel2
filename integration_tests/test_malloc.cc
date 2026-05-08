#include "kio.h"

extern "C" void main() {
    // basic alloc and write
    int* a = new int;
    *a = 42;
    if (*a != 42) { kputs("FAILED: basic alloc\n"); return; }
    delete a;

    // multiple allocations with distinct values
    constexpr int N = 8;
    int* arr[N];
    for (int i = 0; i < N; i++) {
        arr[i] = new int;
        *arr[i] = i * 7;
    }
    for (int i = 0; i < N; i++) {
        if (*arr[i] != i * 7) { kputs("FAILED: multiple allocs\n"); return; }
        delete arr[i];
    }

    // alloc after free (test coalesce)
    int* b = new int;
    *b = 99;
    delete b;
    int* c = new int;
    *c = 100;
    if (*c != 100) { kputs("FAILED: alloc after free\n"); return; }
    delete c;

    kputs("PASSED\n");
}
