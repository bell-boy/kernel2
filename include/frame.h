#pragma once
#include <stdint.h>
#define PAGE_SIZE 4096

extern void init_falloc(uint64_t hhdm_offset, struct limine_memmap_response* memmap_response);
extern void* falloc();
extern void ffree(void* frame);