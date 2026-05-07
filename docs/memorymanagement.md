# Memory Management

Document describing the kernel's memory management features.

Topics Covered:
    1. malloc & free

## Malloc & Free

Within malloc.cc, the kenrel requests a small (~8 MiB) region of zero'd out memory. This memory becomes the allocator used for malloc and free. The current allocator uses an implict free list.

```c++
struct free_list_header {
    uint32_t size;
    bool allocated;
};
```
