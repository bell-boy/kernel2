#include "devices.h"
#include "frame.h"
#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <kstdlib/kio.h>

extern void init_malloc();
extern "C" void main();

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_dtb_request dtb_request = {
  .id = LIMINE_DTB_REQUEST_ID,
  .revision = 6,
  .response = nullptr
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
  .id = LIMINE_HHDM_REQUEST_ID,
  .revision = 0,
  .response = nullptr
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
  .id = LIMINE_MEMMAP_REQUEST_ID,
  .revision = 0,
  .response = nullptr
};


__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;


// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
extern "C" void kmain(void) {
  kstd::kprintf("Entering kmain...\n");
  if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
    kstd::kprintf("[WARN] Kernel base revision not supported.\n");
    kstd::kexit(1);
  }

  kstd::kprintf("Initializing Kernel Heap...\n");
  init_malloc();
  kstd::kputs("Initialized Kernel Heap.\n");

  kstd::kprintf("Initializing Physical Frame Allocator...\n");
  if (hhdm_request.response != nullptr && memmap_request.response != nullptr) {
    init_falloc(hhdm_request.response->offset, memmap_request.response);
    kstd::kprintf("Initialized Physical Frame Allocator.\n");
  } else {
    kstd::kprintf("[WARN] Failed to get HHDM offset or Memmap Regions.\n");
  }

  if (dtb_request.response != nullptr) {
    kstd::kputs("Found Device Tree Blob.\n");
    devices::DeviceTreeParser(reinterpret_cast<const uint32_t *>(dtb_request.response->dtb_ptr));
  } else {
    kstd::kputs("[WARN] Failed to get Device Tree Blob.\n");
  }


  main();
  kstd::kexit(0);
}
