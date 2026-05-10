#include <kstdlib/kio.h>
#include <kstdlib/list.h>

extern "C" void main() {
    kstd::List<int> list;

    list.push_back(2);
    if (list.head()->val != 2) { kstd::kprintf("FAILED: push_back head\n"); return; }
    if (list.tail()->val != 2) { kstd::kprintf("FAILED: push_back tail\n"); return; }

    list.push_front(1);
    list.push_back(3);

    if (list.head()->val != 1) { kstd::kprintf("FAILED: push_front head\n"); return; }
    if (list.tail()->val != 3) { kstd::kprintf("FAILED: push_back tail after push_front\n"); return; }
    if (list.head()->next->val != 2) { kstd::kprintf("FAILED: head next\n"); return; }
    if (list.tail()->prev->val != 2) { kstd::kprintf("FAILED: tail prev\n"); return; }
    if (list.head()->next->next->val != 3) { kstd::kprintf("FAILED: forward order\n"); return; }
    if (list.tail()->prev->prev->val != 1) { kstd::kprintf("FAILED: backward order\n"); return; }

    kstd::kprintf("PASSED\n");
}
