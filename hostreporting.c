#include <linux/module.h>
#include <linux/kernel.h>
#include "hostreporting.h"

void call_cpuid(
    unsigned int* eax,
    unsigned int* ebx,
    unsigned int* ecx,
    unsigned int* edx)
{
    asm volatile(
        "cpuid"
        : "=a" (*eax),
        "=b" (*ebx),
        "=c" (*ecx),
        "=d" (*edx)
        : "0" (*eax),
        "2" (*ecx));
}

void ReportInterruptionToHost(
    _u64 lost,
    int cpu)
{
    unsigned int eax = (unsigned int)(0xF0000000 | (unsigned int)cpu);
    unsigned int ebx = 0;
    unsigned int ecx = (unsigned int)lost;
    unsigned int edx = 0;

    call_cpuid(
        &eax,
        &ebx,
        &ecx,
        &edx);
}