#pragma once

namespace kstd {

extern void kputs(const char* str);
extern void kprintf(const char* fmt, ...);
[[noreturn]] extern void kexit(int code);


};
