#pragma once
#include <stdint.h>
#define PAGE_SIZE 4096

extern inline uint64_t pa2va(uint64_t pa);
extern inline uint64_t va2pa(uint64_t va);

extern void init_falloc(uint64_t hhdm_offset, struct limine_memmap_response* memmap_response);
extern void* falloc();
extern void ffree(void* frame);