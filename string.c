#include "string.h"

void string_init(string_t* this, char* initial_value)
{
    vector_init(&this->vector, 0, sizeof(char));

    string_append(this, initial_value);
}

void string_deallocate(string_t* this)
{
    vector_deallocate(&this->vector);
}

void string_append(string_t* this, char* str)
{
    char* pStr = str;
    char c;

    while (*pStr != '\0')
    {
        c = *pStr++;

        vector_push_back(&this->vector, &c);
    }
}

void string_append_str(string_t* this, string_t* str)
{
    int i;
    char c;

    for (i = 0; i < str->vector.count; ++i)
    {
        c = string_char_at(str, i);
        vector_push_back(&this->vector, &c);
    }
}

char string_char_at(string_t* this, int idx)
{
    return VECTOR_GET_GENERIC(this->vector, idx, char);
}

void string_clear(string_t* this)
{
    vector_clear(&this->vector);
}

char* string_c_str(string_t* this)
{
    char* ptr = vmalloc(this->vector.count + 1);
    memcpy(ptr, this->vector.buffer, this->vector.count);
    ptr[this->vector.count] = '\0';
    return ptr;
}