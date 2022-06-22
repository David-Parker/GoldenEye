#pragma once
#include <linux/vmalloc.h>
#include "globaldef.h"

#define VECTOR_GET_GENERIC(this, idx, type) *((type*)vector_elem_at(&this, idx))

/// <summary>
/// Generic, dynamically expanding array.
/// </summary>
typedef struct vector
{
    byte* buffer;
    _u64 count;
    _u64 size;
    _u64 sizeof_T;
} vector_t;

void vector_init(vector_t* this, _u64 init_size, _u64 sizeof_T);
void vector_deallocate(vector_t* this);
void vector_clear(vector_t* this);
void vector_push_back(vector_t* this, void* data);
void* vector_elem_at(vector_t* this, _u64 index);