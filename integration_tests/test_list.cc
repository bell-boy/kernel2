#include <kstdlib/list.h>
#include <kstdlib/testing.h>

TEST_MAIN({
    kstd::List<int> list;

    list.push_back(2);
    if (*list.head() != 2) { kstd::kprintf("FAILED: push_back head\n"); return; }
    if (*list.tail() != 2) { kstd::kprintf("FAILED: push_back tail\n"); return; }

    list.push_front(1);
    list.push_back(3);

    if (*list.head() != 1) { kstd::kprintf("FAILED: push_front head\n"); return; }
    if (*list.tail() != 3) { kstd::kprintf("FAILED: push_back tail after push_front\n"); return; }
    if (*list.head().next() != 2) { kstd::kprintf("FAILED: head next\n"); return; }
    if (*list.tail().prev() != 2) { kstd::kprintf("FAILED: tail prev\n"); return; }
    if (*list.head().next().next() != 3) { kstd::kprintf("FAILED: forward order\n"); return; }
    if (*list.tail().prev().prev() != 1) { kstd::kprintf("FAILED: backward order\n"); return; }

    kstd::kprintf("PASSED\n");
})
