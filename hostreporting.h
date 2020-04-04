#pragma once
#include "globaldef.h"

void call_cpuid(
    unsigned int eax,
    unsigned int ecx,
    unsigned int ebx,
    unsigned int edx);

void ReportInterruptionToHost(
    _u64 lost,
    int cpu);