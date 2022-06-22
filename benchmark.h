#pragma once
#include "cpufreq.h"
#include "util.h"

#if ENABLE_BENCHMARK == 1
#define NANO_BENCHMARK(code) \
do { \
    _u64 __s; \
    _u64 __e; \
    _u64 __d; \
    __s = _rdtsc_(); \
    code; \
    __e = _rdtsc_(); \
    __d = __e - __s; \
    printk(KERN_INFO "GoldenEye: NANO_BENCHMARK (%s): %llu ticks (%llu ns)", __func__, __d, __d * NANOS_PER_SEC / get_cycles_per_second()); \
} while (0)
#else
#define NANO_BENCHMARK(code) code
#endif