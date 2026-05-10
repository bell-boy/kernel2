#include <kstdlib/debug.h>
#include <kstdlib/kio.h>
#include <kstdlib/shared.h>


struct Box {
    int value;
    int* destructed;

    Box(int value, int* destructed) : value(value), destructed(destructed) {}

    ~Box() {
        (*destructed)++;
    }
};

extern "C" void main() {
    int destructed = 0;

    {
        kstd::SharedPtr<Box> first(new Box(7, &destructed));
        if (first->value != 7) { KPANIC("basic access"); }

        {
            kstd::SharedPtr<Box> copy(first);
            if (copy->value != 7) { KPANIC("copy access"); }

            copy->value = 9;
            if (first->value != 9) { KPANIC("shared pointee"); }

            kstd::SharedPtr<Box> assigned(new Box(3, &destructed));

            assigned = copy;
            if (destructed != 1) { KPANIC("assignment release"); }
            if (assigned->value != 9) { KPANIC("assignment access"); }

            assigned = copy;
            if (destructed != 1) { KPANIC("self assignment failed"); }
            if (assigned->value != 9) { KPANIC("assignment access"); }
        }

        if (destructed != 1) { KPANIC("early delete"); }
    }

    if (destructed != 2) { KPANIC("final delete"); }

    kstd::kprintf("PASSED\n");
}
