#include <stdint.h>
#include <stddef.h>
#include "frame.h"
#include <kstdlib/debug.h>

namespace pagetable {
    struct PageTable {
        uint64_t entries[PAGE_SIZE / sizeof(uint64_t)];
    };

    uint64_t va2pa(void* highest_level_page_table, uint64_t va) {
        // [------16------|--9--|--9--|--9--|--9--|----12----]
        uint64_t l0 = (va >> 39) & 0x1FF;
        uint64_t l1 = (va >> 30) & 0x1FF;
        uint64_t l2 = (va >> 21) & 0x1FF;
        uint64_t l3 = (va >> 12) & 0x1FF;
        uint64_t offset = va & 0xFFF;

        // Each pa2va is a kernel-mode mapping, so it is guaranteed to be accessible
        PageTable* l0pt = reinterpret_cast<PageTable*>(pa2va(reinterpret_cast<uint64_t>(highest_level_page_table)));
        ASSERT(l0pt->entries[l0] & 0b11 == 0b11); // Note: no huge pages yet
        PageTable* l1pt = reinterpret_cast<PageTable*>(pa2va(l0pt->entries[l0] & 0x0000FFFFFFFFF000));
        ASSERT(l1pt->entries[l1] & 0b11 == 0b11);
        PageTable* l2pt = reinterpret_cast<PageTable*>(pa2va(l1pt->entries[l1] & 0x0000FFFFFFFFF000));
        ASSERT(l2pt->entries[l2] & 0b11 == 0b11);
        PageTable* l3pt = reinterpret_cast<PageTable*>(pa2va(l2pt->entries[l2] & 0x0000FFFFFFFFF000));
        ASSERT(l3pt->entries[l3] & 0b11 == 0b11);
        return l3pt->entries[l3] + offset; // raw physical address
    }

    void map_recursive(void* highest_level_page_table, uint64_t va, void* physical_frame, bool read_write, bool device_memory) {

    }
}