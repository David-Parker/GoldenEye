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
#include "proc.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Parker");
MODULE_DESCRIPTION("GoldenEye. Measures hypervisor overhead from within a VM. Based on deschedule written by Steve Deng.");
MODULE_VERSION("0.01");

void disable_interrupts(void);
void enable_interrupts(void);
void measure_interruptions(void*);

// Buffer for output lost time data
char buff[PROCFS_MAX_SIZE];

unsigned long flags = 0;
int secondsToRun = 60;
_u64 g_cyclesPerSec = 0;

// Define module command line arguments
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
    _u64 cyclesPerMicrosecond = g_cyclesPerSec / (_u64)1000000;
    _u64 lostMicros = 0;
    _u64 begin = 0;
    _u64 end = 0;
    _u64 curr = 0;
    _u64 start = 0;
    _u64 last = 0;

    printk(KERN_INFO "GoldenEye: running on core %d", core);

    disable_interrupts();

    begin = __rdtsc();
    end = begin + ((_u64)secondsToRun * g_cyclesPerSec);
    start = begin;
    last = begin;

    do {
        curr = __rdtsc();

        lostMicros = (curr - last) / cyclesPerMicrosecond;

        if (lostMicros > 5)
        {
            ReportInterruptionToHost(lostMicros, core);
        }
        
        last = curr;
    }
    while (curr < end);

    enable_interrupts();
}

static int __init goldeneye_init(void) {
    printk(KERN_INFO "GoldenEye: Starting GoldenEye for %d second(s).\n", secondsToRun);

    g_cyclesPerSec = get_cycles_per_second();
    printk(KERN_INFO "GoldenEye: Cpu frequency: %llu", g_cyclesPerSec);

    buff[0] = 'h';
    buff[1] = 'e';
    buff[2] = 'l';
    buff[3] = 'l';
    buff[4] = 'o';
    buff[5] = '\n';
    buff[6] = '\0';

    if (create_proc() == -1)
    {
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
	 		PROCFS_NAME);

        return -1;
    }

    on_each_cpu(measure_interruptions, NULL, 1);

    return 0;
}

static void __exit goldeneye_exit(void) {
    remove_proc();
    printk(KERN_INFO "GoldenEye: Completed GoldenEye.\n");
}

module_init(goldeneye_init);
module_exit(goldeneye_exit);