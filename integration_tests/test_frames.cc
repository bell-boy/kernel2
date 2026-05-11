#include <kstdlib/list.h>
#include <kstdlib/testing.h>
#include <kstdlib/kio.h>
#include "frame.h"

TEST_MAIN({
    kstd::kprintf("Single Frame Allocation and Free:\n");
    Frame* f = falloc();
    kstd::kprintf("Allocated Physical Frame: %x\n", f);
    ffree(f);
    kstd::kprintf("Freed Physical Frame: %x\n", f);

    // Repeated Allocations and Frees...
    kstd::kprintf("Repeated Frame Allocations and Frees:\n");
    for (int i = 0; i < 5; i++) {
        Frame* f2 = falloc();
        kstd::kprintf("Allocated Physical Frame: %x\n", f2);
        ffree(f2);
        kstd::kprintf("Freed Physical Frame: %x\n", f2);
    }

    // Contiguous Allocations and Frees...
    kstd::kprintf("Contiguous Allocations and Frees:\n");
    Frame* cframes[5];
    for (int i = 0; i < 5; i++) {
        cframes[i] = falloc();
    }
    kstd::kprintf("Finished contiguous allocations, now freeing...\n");
    for (int i = 0; i < 5; i++) {
        ffree(cframes[i]);
    }
    kstd::kprintf("Freed contiguous allocations\n");

    for (int i = 0; i < 5; i++) {
        cframes[i] = falloc();
    }
    ffree(cframes[2]);
    // When we allocate a frame, we should get the page we just freed
    Frame* f3 = falloc();
    if (f3 == cframes[2]) {
        kstd::kprintf("Successfully allocated the recently freed page\n");
    } else {
        kstd::kprintf("FAILED: Allocated at address %x instead", f3);
    }

    kstd::kprintf("PASSED\n");
})