#include <devices/discovery.h>
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
  .revision = 6
};


__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

using init_function_t = void (*)();

extern init_function_t __init_array_start[];
extern init_function_t __init_array_end[];

extern "C" int atexit() {
    return 0;
}

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

  kstd::kprintf("Initializing Global Objects...\n");
  for (init_function_t *f = __init_array_start; f < __init_array_end; f++) {
    (*f)();
  }
  kstd::kputs("Initialized Global Objects.\n");

  if (dtb_request.response != nullptr) {
    kstd::kputs("Found Device Tree Blob.\n");
    devices::DeviceTreeParser(reinterpret_cast<const uint32_t *>(dtb_request.response->dtb_ptr));
  } else {
    kstd::kputs("[WARN] Failed to get Device Tree Blob.\n");
  }


  main();
  kstd::kexit(0);
}
