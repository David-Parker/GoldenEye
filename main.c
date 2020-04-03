// lkm development: https://tldp.org/LDP/lkmpg/2.6/html/x279.html

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timekeeping.h>
#include "cpufreq.h"
#include "globaldef.h"
#include "hostreporting.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Parker");
MODULE_DESCRIPTION("GoldenEye. Measures hypervisor overhead from within a VM.");
MODULE_VERSION("0.01");

void measure_interruptions(void*);

int secondsToRun = 60;
int g_cyclesThreshold = 10000;
_u64 g_cyclesPerSec = 0;

module_param(secondsToRun, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(secondsToRun, "Seconds to run GoldenEye.");

void measure_interruptions(void* info)
{
    int core = smp_processor_id();
    _u64 now = ktime_get_real_ns();
    _u64 end = now + ((_u64)secondsToRun * NANOS_PER_SEC);
    _u64 offset = 0;
    _u64 cyclesPerMicrosecond = g_cyclesPerSec / (_u64)1000000;
    _u64 lostMicros = 0;
    _u64 begin = 0;
    _u64 curr = 0;
    _u64 start = 0;
    _u64 last = 0;
    _u32 aux = 0;

    asm volatile("cli" :::); // disable interrupts

    begin = __rdtscp(&aux);
    start = begin;
    last = begin;

    do {
        curr = __rdtscp(&aux);

        if (last + g_cyclesThreshold > curr)
        {
            last = curr;
        }
        else // We exceeded 10,000 cycles
        { 
            offset = 0;

            if (curr >= last)
            {
                offset = curr - last;
            }

            start = curr;
            last = curr;

            // offset is in tsc clock cycles
            lostMicros = offset / cyclesPerMicrosecond;

            // Only report skips larger than 10us
            if (lostMicros >= 10)
            {
                printk(KERN_INFO "core %d: Lost time: %llu", core, lostMicros);
                ReportInterruptionToHost(lostMicros, core);
            }
        }
    }
    while (ktime_get_real_ns() < end);

    asm volatile("sti" :::); // enable interrupts
}

static int __init goldeneye_init(void) {
    printk(KERN_INFO "Starting GoldenEye for %d second(s).\n", secondsToRun);

    g_cyclesPerSec = get_cycles_per_second();
    printk(KERN_INFO "Cpu frequency: %llu", g_cyclesPerSec);

    on_each_cpu(measure_interruptions, NULL, 1);

    return 0;
}

static void __exit goldeneye_exit(void) {
    printk(KERN_INFO "Completed GoldenEye.\n");
}

module_init(goldeneye_init);
module_exit(goldeneye_exit);