#pragma once
#include <linux/kernel.h>
#include "string.h"
#include "vector.h"

#define JSON_GET_GENERIC(this, type) *((type*)this->value)

/// <summary>
/// Json serializer for creating custom JSON payloads.
/// </summary>
typedef enum json_type
{
    JsonObject,
    JsonArray,
    JsonString,
    JsonNumber,
    JsonReal,
    JsonBool,
    JsonNull
} json_type_t;

typedef struct json_element
{
    json_type_t type;
    vector_t children;
    string_t name;
    byte value[8];
    struct json_element* parent;
} json_element_t;

typedef struct json_serializer
{
    json_element_t* root;
    string_t buffer;
} json_serializer_t;

// json_element
void json_element_init(json_element_t* this, char* name, json_type_t type, json_element_t* parent);
void json_element_deallocate(json_element_t* this);

// json_serializer
void json_serializer_init(json_serializer_t* this);
void json_serializer_deallocate(json_serializer_t* this);
void json_serializer_add_tabs(json_serializer_t* this, int depth);
json_element_t* json_serializer_add_element(json_serializer_t* this, json_element_t* parent, char* name, json_type_t type);
json_element_t* json_serializer_add_object(json_serializer_t* this, json_element_t* parent, char* name);
json_element_t* json_serializer_add_array(json_serializer_t* this, json_element_t* parent, char* name);
json_element_t* json_serializer_add_value(json_serializer_t* this, json_element_t* parent, char* name, json_type_t type, void* value, int sizeof_T);
json_element_t* json_serializer_add_string(json_serializer_t* this, json_element_t* parent, char* name, char* value);
json_element_t* json_serializer_add_number(json_serializer_t* this, json_element_t* parent, char* name, long long value);
// json_real is removed because Linux on ARM64 does not support floating point numbers in kernel space
/* json_element_t* json_serializer_add_real(json_serializer_t* this, json_element_t* parent, char* name, double value); */
json_element_t* json_serializer_add_bool(json_serializer_t* this, json_element_t* parent, char* name, bool value);
json_element_t* json_serializer_add_null(json_serializer_t* this, json_element_t* parent, char* name);
char* json_serializer_to_string(json_serializer_t* this, bool pretty);

// util
int snprintf(char* buf, size_t size, const char* fmt, ...);