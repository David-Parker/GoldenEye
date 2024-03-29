#include <linux/module.h>
#include <linux/kernel.h>

#include "cpufreq.h"
#include "util.h"

#if defined(__x86_64__)
#include "arch/x64/cpufreq.c"
#elif defined(__aarch64__)
#include "arch/ARM64/cpufreq.c"
#endif

_u64 get_cycles_per_second_measured()
{
    _u64 cyclesPerSec = 0;
    _u64 gap = 0;
    _u32 index;
    _u64 time;
    _u64 time1;
    _u64 time2;
    _u64 tsc0;
    _u64 tsc1;
    _u64 tsc2;

    for (index = 0; index < 3; index += 1)
    {
        tsc0 = _rdtsc_();
        time = ktime_get_real_ns();

        do 
        {
            time1 = ktime_get_real_ns();
        } 
        while (time1 == time);

        tsc1 = _rdtsc_();

        do 
        {
            time2 = ktime_get_real_ns();
        } 
        while (time2 - time1 < 1 * NANOS_PER_SEC);

        tsc2 = _rdtsc_();

        if ((gap == 0) ||
            ((tsc2 > tsc1) && (tsc2 - tsc0 < gap))) 
        {

            gap = tsc2 - tsc0;
            cyclesPerSec = (tsc2 - tsc1) * (_u64)(NANOS_PER_SEC) / (time2 - time1);

            // round to nearest mhz
            cyclesPerSec += 1000000 / 2;
            cyclesPerSec -= (cyclesPerSec % 1000000);
        }
    }

    return cyclesPerSec;
}