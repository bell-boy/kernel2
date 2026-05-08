#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE 8 * 1024 * 1024

char heap[HEAP_SIZE];

struct free_list_header {
  size_t size; // includes the size of the header
  bool used;
};

const size_t MIN_FREE_BLOCK_SIZE_DATA = 2*sizeof(free_list_header); // Does not include the size of the header

void init_malloc() {
  free_list_header* first_header = reinterpret_cast<free_list_header*>(heap);
  first_header->size = HEAP_SIZE;
  first_header->used = false;
}

/**
 * Allocates a specified number of bytes for kernel use, aligned to a specified amount of bytes.
 * ONLY VALID FOR SINGLE CORE
 * @param size      The number of bytes to allocate within the kernel heap
 * @param al        The number of bytes to align the returned address to.
 * @returns         A pointer to an allocated region of the kernel heap. (Guaranteed to be divisible by al)
 */
void* kmalloc(size_t size, size_t al) {
  free_list_header *current = reinterpret_cast<free_list_header*>(heap);
  while(current < reinterpret_cast<free_list_header*>(heap) + HEAP_SIZE) {
    // find the first aligned address within this block 
    // current design deals with alignment requirements by just wasting the first part of any block wr find
    size_t base_addr = (size_t) current + sizeof(free_list_header);
    size_t al_offset = al - (base_addr % al);
    if (current->used || (al_offset + size > current->size - sizeof(free_list_header))) {
      current = current + current->size; // Move to the next header
      continue;
    }
    // First-fit policy, carve out this region and mark it as used
    size_t aligned_addr = base_addr + al_offset;
    size_t old_size = current->size;
    current->used = true;

    // Check if there is enough room to make a new free list block
    if (old_size - (sizeof(free_list_header) + al_offset + size) > sizeof(free_list_header) + MIN_FREE_BLOCK_SIZE_DATA) {
      // Insert a new free list header
      current->size = sizeof(free_list_header) + al_offset + size; // Carve out reserved portion
      free_list_header *post = reinterpret_cast<free_list_header*>(current + current->size); 
      post->size = old_size - current->size;
      post->used = false;
    }
    return reinterpret_cast<void*>(aligned_addr);
  }
  // TODO: We need a kernel panic here
  return nullptr;
}

// TODO: currently this silently fails on ptrs not returned by kmalloc inside the heap region
// TODO: currently not coalescing
void kfree(void *ptr) {
  // find the block that fully contains this pointer 
  free_list_header *current = reinterpret_cast<free_list_header*>(heap);
  size_t ptr_addr = reinterpret_cast<size_t>(ptr);
  while(current < reinterpret_cast<free_list_header*>(heap) + HEAP_SIZE) {
    size_t curr_addr = reinterpret_cast<size_t>(current);
    if (curr_addr < ptr_addr && ptr_addr < curr_addr + current->size) {
      if (current->used) { 
        current->used = false;
        return;
      } else {
        // TODO: panic, we just had a double free
        return;
      }
    } 
  }
  // TODO: panic, invalid free
}

void* operand new(std::size_t count) {
  return kmalloc(count, 16);
}

void* operand new[](std::size_t count) {
  return kmalloc(count, 16);
}


void* operand new(std::size_t count, std::align_val_t al) {
  return kmalloc(count, static_cast<size_t>(al));
}

void* operand new[](std::size_t count, std::align_val_t al) {
  return kmalloc(count, static_cast<size_t>(al));
}
// scalar delete

void operator delete(void* p) noexcept {
  kfree(p);
}

void operator delete(void* p, size_t n) noexcept {
  kfree(p);
}

void operator delete(void* p, std::align_val_t a) noexcept {
  kfree(p);
}

void operator delete(void* p, size_t n, std::align_val_t a) noexcept {
  kfree(p);
}

// array delete

void operator delete[](void* p) noexcept {
  kfree(p);
}

void operator delete[](void* p, size_t n) noexcept {
  kfree(p);
}

void operator delete[](void* p, std::align_val_t a) noexcept {
  kfree(p);
}

void operator delete[](void* p, size_t n, std::align_val_t a) noexcept {
  kfree(p);
}
