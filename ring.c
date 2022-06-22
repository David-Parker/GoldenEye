#include "ring.h"

__always_inline void ring_reset(ring_buffer_t* this)
{
    this->start = 0;
    this->end = 0;
}

__always_inline void ring_push(ring_buffer_t* this, _u64 val)
{
    this->buffer[this->end] = val;
    this->end = ring_next(this->end);

    if (this->end == this->start)
    {
        this->start = ring_next(this->start);
    }
}

__always_inline int ring_next(int idx)
{
    return (idx + 1) % (RING_BUF_MAX + 1);
}