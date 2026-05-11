#pragma once
#include <stdint.h>
#define PAGE_SIZE 4096

struct Frame {
    Frame* next;
    char padding[PAGE_SIZE - sizeof(Frame*)];
};

extern void init_falloc(uint64_t hhdm_offset, struct limine_memmap_response* memmap_response);
extern Frame* falloc();
extern void ffree(Frame* frame);