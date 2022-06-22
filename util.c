#include "util.h"

#if defined(__x86_64__)
#include "arch/x64/util.c"
#elif defined(__aarch64__)
#include "arch/ARM64/util.c"
#endif

__always_inline _u64 _bsr64_(_u64 value) // BitScanReverse 64 bit
{
    // clz returns the number of leading zeros
    // to get highest set bit we subtract this number from 63
    // 2^0 = 0, 2^10 = 10, etc.
    return 63 - __builtin_clzll(value);
}