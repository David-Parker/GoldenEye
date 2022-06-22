#pragma once
#include "vector.h"

/// <summary>
/// Mutable string
/// </summary>
typedef struct string
{
    vector_t vector;
} string_t;

void string_init(string_t* this, char* initial_value);
void string_deallocate(string_t* this);
void string_append(string_t* this, char* str);
void string_append_str(string_t* this, string_t* str);
char string_char_at(string_t* this, int idx);
void string_clear(string_t* this);
char* string_c_str(string_t* this);