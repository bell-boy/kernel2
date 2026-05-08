#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "kio.h"

extern void init_malloc();

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_dtb_request dtb_request = {
  .id = LIMINE_DTB_REQUEST_ID,
  .revision = 6
};


__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;


// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
      __asm__ volatile ("wfi");
    }
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
extern "C" void kmain(void) {
  kprintf("Entering kmain...\n");
  if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
    kputs("Kernel base revision not supported.\n");
    hcf();
  }

  // bring up devices
  if (dtb_request.response != nullptr) {
    kputs("Found Device Tree Blob.\n");
  } else {
    kputs("[WARN] Failed to get Device Tree Blob.\n");
  }
  // load the entry point
  // switch to user

  kprintf("Hello, World!\n");

  kprintf("Initializing Kernel Heap...\n");
  init_malloc();
  kputs("Initialized Kernel Heap.\n");

  int *a = new int;
  *a = 10;
  if (*a == 10) { 
    kprintf("Kernel Malloc Works!\n");
  }
  delete a;
  kprintf("Kernel Free Works!\n");
  hcf();
}
