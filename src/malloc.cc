#include <stdint.h>
#include <stddef.h>

#define HEAP_SIZE 8 * 1024 * 1024

char heap[HEAP_SIZE];

struct free_list_header {
  size_t size; // includes the size of the header
  bool used;
}

void init_malloc() {
  free_list_header* first_header = heap;
  first_header->size = HEAP_SIZE;
  first_header->used = false;
}


void* kmalloc(size_t size, size_t al) {
  // find a free block aligned to size with the size of size
  free_list_header *current = heap;
  while(current < heap + HEAP_SIZE) {
    // find the first aligned address within this block 
    // current design deals with alignment requirements by just wasting the first part of any block wr find
    size_t base_addr = (size_t) current + sizeof(free_list_header);
    size_t al_offset = al - (base_addr % al);
    if (al_offset + size > current->size - sizeof(free_list_header) || current->used) {
      current = base_addr + free_list_header->size;
      continue;
    }
    // we've found it, carve out the region then return this addr
    size_t aligned_addr = base_addr + al_offset;
    size_t old_size = current->size;
    current->size = al_offset + size + sizeof(free_list_header); 
    current->used = true;
    free_list_header *post = (size_t) current + current->size; 
    post->size = old_size - current->size;
    post->used = false;
    return aligned_addr;
  }
  return nullptr;
}

// TODO: currently this silently fails on ptrs not returned by kmalloc inside the heap region
// TODO: currently not coalescing
void kfree(void *ptr) {
  // find the block that fully contains this pointer 
  free_list_header *current = heap;
  size_t ptr_addr = ptr;
  while(current < heap + HEAP_SIZE) {
    size_t curr_addr = current;
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
