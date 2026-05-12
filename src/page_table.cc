#include <stdint.h>
#include <stddef.h>
#include "frame.h"

class PageTable {
    uint64_t entries[PAGE_SIZE / sizeof(uint64_t)];

    uint64_t va2pa(uint64_t va) {
        // Mask off the lower bits
        
    }
};