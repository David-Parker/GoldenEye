#pragma once

#include <linux/module.h>
#include <linux/kernel.h>
#include "globaldef.h"

#define RING_BUF_MAX 64

typedef struct ring_buffer
{
    _u64 buffer[RING_BUF_MAX + 1];
    int start;
    int end;
} ring_buffer_t;

void ring_reset(ring_buffer_t* this);
void ring_push(ring_buffer_t* this, _u64 val);
int ring_next(int idx);