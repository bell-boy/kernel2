#pragma once
#include <stdint.h>
#include <stddef.h>

extern void* kmmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset);
extern void kmunmap(void* addr, size_t length);