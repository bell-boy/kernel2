#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE 8 * 1024 * 1024
#define HEAP_AL 16

char heap[HEAP_SIZE];

struct alignas(HEAP_AL) free_list_header {
  size_t size; // Includes the size of the header and footer
  bool used;
};

struct alignas(HEAP_AL) free_list_footer {
  size_t size; // Must always be the same size as the corresponding header
};

// Does not include the size of the header or footer
const size_t MIN_FREE_BLOCK_SIZE_DATA = sizeof(free_list_header);

void init_malloc() {
  free_list_header* first_header = reinterpret_cast<free_list_header*>(heap);
  first_header->size = HEAP_SIZE;
  first_header->used = false;
  free_list_footer* first_footer = reinterpret_cast<free_list_footer*>(heap + HEAP_SIZE - sizeof(free_list_footer));
  first_footer->size = HEAP_SIZE;
}

/**
 * Allocates a specified number of bytes for kernel use.
 * ONLY VALID FOR SINGLE CORE
 * @param size_      The number of bytes to allocate within the kernel heap
 * @returns         A pointer to an allocated region of the kernel heap. (Guaranteed to be divisible by HEAP_AL)
 */
void* kmalloc(size_t size_) {
  size_t size = ((size_ + HEAP_AL - 1) / HEAP_AL) * HEAP_AL; // Round up to the nearest multiple of HEAP_AL
  free_list_header *current = reinterpret_cast<free_list_header*>(heap);
  while(current < reinterpret_cast<free_list_header*>(heap) + HEAP_SIZE) {
    // find the first aligned address within this block 
    // current design deals with alignment requirements by just wasting the first part of any block wr find
    size_t base_addr = (size_t) current + sizeof(free_list_header);
    if (current->used || (size > current->size - sizeof(free_list_header) - sizeof(free_list_footer))) {
      current = current + current->size; // Move to the next header
      continue;
    }
    // First-fit policy
    current->used = true;

    // Check if there is enough room to make a new free list block
    if (current->size - (sizeof(free_list_header) + size + sizeof(free_list_footer)) > sizeof(free_list_header) + MIN_FREE_BLOCK_SIZE_DATA + sizeof(free_list_footer)) {
      // Carve out the reserved, mallocced portion
      size_t old_size = current->size;
      current->size = sizeof(free_list_header) + size + sizeof(free_list_footer);
      // Update the footer
      free_list_footer *current_footer = reinterpret_cast<free_list_footer*>(current + current->size - sizeof(free_list_footer));
      current_footer->size = current->size;
      // Add the new free block
      free_list_header *post = reinterpret_cast<free_list_header*>(current + current->size); 
      post->size = old_size - current->size;
      post->used = false;
      // Update the post footer
      free_list_footer *post_footer = reinterpret_cast<free_list_footer*>(post + post->size - sizeof(free_list_footer));
      post_footer->size = post->size;
    }
    return reinterpret_cast<void*>(base_addr);
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
