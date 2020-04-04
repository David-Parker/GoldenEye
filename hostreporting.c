#include <linux/module.h>
#include <linux/kernel.h>
#include "hostreporting.h"

__always_inline void call_cpuid(
    unsigned int eax,
    unsigned int ecx,
    unsigned int ebx,
    unsigned int edx)
{
    asm volatile(
        "cpuid"
        : "=a" (eax),
        "=b" (ebx),
        "=c" (ecx),
        "=d" (edx)
        : "0" (eax),
        "2" (ecx));
}

__always_inline void ReportInterruptionToHost(
    _u64 lost,
    int cpu)
{
    unsigned int eax = (unsigned int)(0xF0000000 | (unsigned int)cpu);
    unsigned int ecx = (unsigned int)lost;

    call_cpuid(
        eax,
        ecx,
        0,
        0);
}