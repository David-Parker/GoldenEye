// All about TSC: https://unix4lyfe.org/benchmarking/
__always_inline _u64 _rdtsc_()
{
    // We don't use the intrinsic __rdtscp(int* aux) because it writes ecx (we ignore) to a memory location which can add latency.
    unsigned lo, hi;
    asm volatile(
        "rdtscp"
        : "=a" (lo), "=d" (hi)
        :
        : "%ecx"); // ignore ecx value and tell compiler it's being clobbered

    return ((unsigned long)lo) | (((unsigned long)hi) << 32);
}