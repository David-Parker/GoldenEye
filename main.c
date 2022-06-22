// lkm development: https://tldp.org/LDP/lkmpg/2.6/html/x279.html

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timekeeping.h>
#include <linux/interrupt.h>
#include <linux/irqflags.h>

#include "benchmark.h"
#include "cpufreq.h"
#include "globaldef.h"
#include "losttime.h"
#include "proc.h"
#include "util.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("David Parker");
MODULE_DESCRIPTION("GoldenEye: a tool to measure missing CPU cycles.");
MODULE_VERSION("1.0.5");

void disable_interrupts(void);
void enable_interrupts(void);
void measure_interruptions(void*);

// What is the smallest sized cpu skip we record?
#define MIN_RES_MICROS 1

// Buffer for output lost time data
struct LostTimes g_lostTimes = {};

// Conversion for TSC cycles to seconds, set from cpufreq.c
_u64 g_cyclesPerSec = 0;

// Define module command line arguments
int secondsToRun = 60;
int prettyPrint = 0;

module_param(secondsToRun, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(secondsToRun, "Seconds to run GoldenEye.");
module_param(prettyPrint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(prettyPrint, "Should output file be formatted pretty? 1 = yes, 0 = no.");

int calibrate__rdtsc__overhead(void)
{
    _u64 begin = 0;
    _u64 end = 0;
    int diff = 0;
    int _rdtsc_Overhead = __INT32_MAX__;
    int i;

    // Calibrate read time stamp counter overhead.
    for (i = 0; i < 500000; ++i)
    {
        begin = _rdtsc_();
        end = _rdtsc_();
        diff = end - begin;
        _rdtsc_Overhead = min(_rdtsc_Overhead, diff);
    }

    // Go one cycle less to guarauntee no false readings later
    _rdtsc_Overhead -= 1;

    if (_rdtsc_Overhead < 0)
    {
        _rdtsc_Overhead = 0;
    }

    return _rdtsc_Overhead;
}

void measure_interruptions(void* info)
{
    int core = smp_processor_id();
    struct LostTime* pLostTime;
    _u64 cyclesPerMicrosecond = g_cyclesPerSec / (_u64)1000000;
    _u64 lostMicros = 0;
    _u64 begin = 0;
    _u64 end = 0;
    _u64 curr = 0;
    _u64 start = 0;
    _u64 last = 0;
    _u64 second = 0;
    _s64 diff = 0;
    _u64 accumulator = 0;
    _u32 _rdtsc_Overhead = 0;
    _u64 index = 0;
    unsigned long flags = 0;

    if (core >= MAX_CORES)
    {
        return;
    }

    pLostTime = &g_lostTimes.Times[core];
    pLostTime->Count = 0;

    printk(KERN_INFO "GoldenEye: running on core %d", core);

    local_irq_save(flags);

    _rdtsc_Overhead = calibrate__rdtsc__overhead();
    pLostTime->TscOverhead = _rdtsc_Overhead;

    secondsToRun = min(secondsToRun, MAX_SECONDS);

    begin = _rdtsc_();
    end = begin + ((_u64)secondsToRun * g_cyclesPerSec);
    start = begin;
    last = begin;
    second = begin;

    /* 
        -- Nano-Benchmark results (x86_64) --
        TSC Overhead: 35 ticks (_rdtsc_)
        TSC Freq: 3491912000hz

        _bsr64_: 8 ticks (2ns)
        _rdtsc_: 35 ticks (10ns)
        Total: 143 ticks (41ns)

        -- Nano-Benchmark results (aarch64) --
        TSC Freq: 25000000hz
        Total: 1 tick (40ns)
    */
    do {
        NANO_BENCHMARK(
            curr = _rdtsc_();
            diff = curr - last - _rdtsc_Overhead;

            if (unlikely(diff < 0))
            {
                // Catches rare and egregious case of TSC jumping backwards. This is a hardware bug.
                pLostTime->Drift += diff;
            }
            else
            {
                lostMicros = diff / cyclesPerMicrosecond;

                // Single skip of at least 1us
                if (unlikely(lostMicros >= MIN_RES_MICROS))
                {
                    accumulator += lostMicros;

                    index = _bsr64_(lostMicros);
                    pLostTime->Histogram[index]++;
                    pLostTime->HistogramSum[index] += lostMicros;
                }

                // Another second has passed
                if (unlikely((curr - second) >= g_cyclesPerSec))
                {
                    if (likely(pLostTime->Count < MAX_SECONDS))
                    {
                        pLostTime->Lost[pLostTime->Count] = accumulator;
                        pLostTime->Count++;
                        accumulator = 0;
                        second = curr;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            last = curr;
        );
    }
    while (curr < end);

    local_irq_restore(flags);
}

static int __init goldeneye_init(void) {
    printk(KERN_INFO "GoldenEye: Starting GoldenEye for %d second(s).\n", secondsToRun);

    g_cyclesPerSec = get_cycles_per_second();
    printk(KERN_INFO "GoldenEye: Tsc frequency: %llu", g_cyclesPerSec);

    // allows data to be read from userspace
    if (proc_create_file() == -1)
    {
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
	 		PROCFS_NAME);

        return -1;
    }

    g_lostTimes.StartTimeNs = ktime_get_real_ns();
    g_lostTimes.Cores = num_online_cpus();

    on_each_cpu(measure_interruptions, NULL, 0);

    return 0;
}

static void __exit goldeneye_exit(void) {
    proc_remove_file();
    printk(KERN_INFO "GoldenEye: Completed GoldenEye.\n");
}

module_init(goldeneye_init);
module_exit(goldeneye_exit);