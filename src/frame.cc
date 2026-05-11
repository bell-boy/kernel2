#include "frame.h"
#include "limine.h"
#include <stdint.h>
#include <kstdlib/string.h>
#include <kstdlib/debug.h>

uint64_t HHDM_OFFSET = 0;
inline uint64_t pa2va(uint64_t pa) { return pa + HHDM_OFFSET; }
inline uint64_t va2pa(uint64_t va) { return va - HHDM_OFFSET; }

Frame* free_head;
size_t frames_alloced;

void init_falloc(uint64_t hhdm_offset, struct limine_memmap_response* memmap_response) {
    HHDM_OFFSET = hhdm_offset;

    free_head = nullptr;
    bool first_frame_flag = false;
    frames_alloced = 0;
    Frame* prev = nullptr;
    for (uint64_t entry_num = 0; entry_num < memmap_response->entry_count; entry_num++) {
        limine_memmap_entry* entry = memmap_response->entries[entry_num];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            // Create Free List Entries
            ASSERT(entry->base % PAGE_SIZE == 0);
            for (uint64_t phys_addr = entry->base; entry->base + entry->length - phys_addr >= PAGE_SIZE; phys_addr += PAGE_SIZE) {
                Frame* curr = reinterpret_cast<Frame*>(pa2va(phys_addr));
                curr->next = nullptr;
                if (!first_frame_flag) {
                    first_frame_flag = true;
                    free_head = curr;
                }
                if (prev != nullptr) {
                    prev->next = curr;
                }
                prev = curr;
            }
        }
    }
}

/**
 * Allocates a single 4K Physical Frame
 */
Frame* falloc() {
    if (free_head == nullptr) {
        KPANIC("No more frames");
    }
    Frame* f = free_head;
    free_head = free_head->next;
    frames_alloced++;
    memset(reinterpret_cast<void*>(f), 0, sizeof(Frame));
    return f;
}

/**
 * Frees a single 4K Physical Frame
 */
void ffree(Frame* frame) {
    ASSERT(reinterpret_cast<uint64_t>(frame) % PAGE_SIZE == 0);
    frames_alloced--;
    frame->next = free_head;
    free_head = frame;
}