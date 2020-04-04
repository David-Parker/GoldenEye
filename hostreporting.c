#include <linux/module.h>
#include <linux/kernel.h>
#include "hostreporting.h"

__always_inline void call_cpuid(
    unsigned int eax,
    unsigned int ecx)
{
    asm volatile(
        "cpuid"
        :   "=a" (eax),
            "=c" (ecx));
}

__always_inline void ReportInterruptionToHost(
    _u64 lost,
    int cpu)
{
    unsigned int eax = (unsigned int)(0xF0000000 | (unsigned int)cpu);
    unsigned int ecx = (unsigned int)lost;

    call_cpuid(
        eax,
        ecx);
}