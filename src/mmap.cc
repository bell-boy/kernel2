#include "mmap.h"

/**
 * Maps a region of memory into the current process' virtual address space.
 * @param addr          The VIRTUAL, PAGE ALIGNED address to map a page to (if nullptr, will find an address arbitrarily)
 * @param length        The number of contiguous bytes of memory to map, does not need to be page aligned
 * @param prot          PROT_READ, PROT_WRITE, PROT_EXEC, PROT_NONE
 * @param flags         MAP_SHARED, MAP_PRIVATE, MAP_ANONYMOUS
 * @param fd            The file descriptor to map, if applicable (TODO: implement filesystem)
 * @param offset        The offset into the file to map (TODO: implement filesystem)
 * @returns             The page-aligned address that the map occurred at, or nullptr if failed.
 */
void* kmmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset) {

}

/**
 * Unmaps a region of memory from the current process' virtual address space.
 * @param addr          The VIRTUAL, PAGE ALIGNED address to unmap (must not be nullptr)
 * @param length        The region of memory to unmap, unmaps all pages within the region
 */
void kmunmap(void* addr, size_t length){

}