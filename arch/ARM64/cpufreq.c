_u64 get_cycles_per_second()
{
    _u64 val;

    asm volatile(
        "mrs %0, cntfrq_el0"
        : "=r" (val));

    return val;
}