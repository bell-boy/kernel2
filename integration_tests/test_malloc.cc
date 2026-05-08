#include <kstdlib/kio.h>

extern "C" void main() {
    // Basic Malloc/Free
    int* a = new int;
    *a = 42;
    if (*a != 42) { kstd::kprintf("FAILED: basic malloc\n"); return; }
    delete a;

    // Multiple Mallocs
    constexpr int N = 8;
    int* arr[N];
    for (int i = 0; i < N; i++) {
        arr[i] = new int;
        *arr[i] = i * 7;
    }
    for (int i = 0; i < N; i++) {
        if (*arr[i] != i * 7) { kstd::kprintf("FAILED: multiple mallocs\n"); return; }
        delete arr[i];
    }

    // Malloc after Free (test coalesce)
    int* b = new int;
    *b = 99;
    delete b;
    int* c = new int;
    *c = 100;
    if (*c != 100) { kstd::kprintf("FAILED: malloc after free\n"); return; }
    delete c;

    // Malloc Entire Heap
    char* heap = new char[8 * 1024 * 1024 - 32]; // HEAP_SIZE - sizeof(header) - sizeof(footer)
    delete[] heap;
    int* d = new int;
    *d = 101;
    if (*d != 101) { kstd::kprintf("FAILED: malloc entire heap\n"); return; }
    delete d;

    kstd::kprintf("PASSED\n");
}
