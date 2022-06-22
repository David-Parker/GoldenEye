#include <linux/timekeeping.h>
#include <linux/cpufreq.h>

_u64 get_cycles_per_second()
{
    return tsc_khz * 1000;
}