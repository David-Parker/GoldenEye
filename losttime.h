#pragma once
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "globaldef.h"

#define MAX_CORES 128
#define MAX_SECONDS 86400

struct LostTime
{
    _u64 Lost[MAX_SECONDS];
    _u32 Count;
};

struct LostTimes
{
    _u64 StartTimeNs;
    _u32 Cores;
    struct LostTime Times[MAX_CORES];
};