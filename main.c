// lkm development: https://tldp.org/LDP/lkmpg/2.6/html/x279.html

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timekeeping.h>
#include <linux/interrupt.h>
#include <linux/irqflags.h>

#include "cpufreq.h"
#include "globaldef.h"
#include "hostreporting.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Parker");
MODULE_DESCRIPTION("GoldenEye. Measures hypervisor overhead from within a VM. Based on deschedule written by Steve Deng.");
MODULE_VERSION("0.01");

void disable_interrupts(void);
void enable_interrupts(void);
void measure_interruptions(void*);

static unsigned long flags = 0;
int secondsToRun = 60;
_u64 g_cyclesPerSec = 0;

module_param(secondsToRun, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(secondsToRun, "Seconds to run GoldenEye.");

void disable_interrupts()
{
    local_irq_save(flags);
    local_irq_disable();
}

void enable_interrupts()
{
     local_irq_restore(flags);
}

void measure_interruptions(void* info)
{
    int core = smp_processor_id();

    printk(KERN_INFO "GoldenEye: running on core %d", core);

    _u64 offset = 0;
    _u64 cyclesPerMicrosecond = g_cyclesPerSec / (_u64)1000000;
    _u64 lostMicros = 0;
    _u64 begin = 0;
    _u64 end = 0;
    _u64 curr = 0;
    _u64 start = 0;
    _u64 last = 0;

    disable_interrupts();

    begin = __rdtsc();
    end = begin + ((_u64)secondsToRun * g_cyclesPerSec);
    start = begin;
    last = begin;

    do {
        curr = __rdtsc();

        offset = curr - last;

        if (offset > cyclesPerMicrosecond)
        {
            // offset is in tsc clock cycles
            lostMicros = offset / cyclesPerMicrosecond;

            // Only report skips larger than 1us
            if (lostMicros > 0)
            {
                // printk(KERN_INFO "GoldenEye: core %d: Lost time: %llu us", core, lostMicros); // for debugging
                ReportInterruptionToHost(lostMicros, core);
            }

            last = __rdtsc();
        }
        else
        {
            last = curr;
        }
    }
    while (curr < end);

    enable_interrupts();
}

static int __init goldeneye_init(void) {
    printk(KERN_INFO "GoldenEye: Starting GoldenEye for %d second(s).\n", secondsToRun);

    g_cyclesPerSec = get_cycles_per_second();
    printk(KERN_INFO "GoldenEye: Cpu frequency: %llu", g_cyclesPerSec);

    on_each_cpu(measure_interruptions, NULL, 1);

    return 0;
}

static void __exit goldeneye_exit(void) {
    printk(KERN_INFO "GoldenEye: Completed GoldenEye.\n");
}

module_init(goldeneye_init);
module_exit(goldeneye_exit);