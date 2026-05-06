#include <kio.h>
void kputs(const char *str) {
  constexpr int SYS_WRITE0 = 0x04;
  register int w0 asm("w0") = SYS_WRITE0;
  register const char *x1 asm("x1") = str;

  // AArch64 semihosting uses x0/x1 and HLT #0xf000.
  asm volatile (
      "hlt #0xf000"
      : "+r"(w0)
      : "r"(x1)
      : "memory"
  );
}
