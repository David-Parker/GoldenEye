#pragma once
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "globaldef.h"

#define MAX_CORES 256
#define MAX_SECONDS 14400 // 4hr

struct LostTime
{
    _u64 Lost[MAX_SECONDS];
    _s64 Drift;
    _u32 TscOverhead;
    _u32 Count;
    _u64 Histogram[64];
    _u64 HistogramSum[64];
};

struct LostTimes
{
    _u64 StartTimeNs;
    _u32 Cores;
    struct LostTime Times[MAX_CORES];
};