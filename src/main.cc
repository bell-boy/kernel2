#include "kio.h"

/**
 * This is when running the kernel in non-test mode
 */
extern "C" void main() {
  int *a = new int;
  *a = 10;
  if (*a == 10) {
    kprintf("Kernel Malloc Works!\n");
  }
  delete a;
  kprintf("Kernel Free Works!\n");
}
