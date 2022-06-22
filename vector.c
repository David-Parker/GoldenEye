#include "vector.h"

void vector_init(vector_t* this, _u64 init_size, _u64 sizeof_T)
{
    if (init_size == 0)
    {
        init_size = 1;
    }

    this->size = init_size;
    this->count = 0;
    this->buffer = vmalloc(init_size * sizeof_T);
    this->sizeof_T = sizeof_T;
}

void vector_deallocate(vector_t* this)
{
    vfree(this->buffer);
}

void vector_clear(vector_t* this)
{
    this->count = 0;
    memset(this->buffer, 0, this->size * this->sizeof_T);
}

void vector_push_back(vector_t* this, void* data)
{
    _u64 byte_offset;
    _u64 new_size;
    byte* new_buffer;

    // Resize array
    if (this->count == this->size)
    {
        new_size = this->size * 2;
        new_buffer = vmalloc(new_size * this->sizeof_T);
        memcpy(new_buffer, this->buffer, this->size * this->sizeof_T);
        vfree(this->buffer);
        this->size = new_size;
        this->buffer = new_buffer;
    }

    byte_offset = this->count * this->sizeof_T;

    memcpy(&this->buffer[byte_offset], data, this->sizeof_T);

    this->count++;
}

void* vector_elem_at(vector_t* this, _u64 index)
{
    if (index >= this->count)
    {
        return NULL;
    }

    return &this->buffer[index * this->sizeof_T];
}