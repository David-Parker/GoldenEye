#include "json.h"

#pragma GCC diagnostic ignored "-Wswitch"

void json_element_init(json_element_t* this, char* name, json_type_t type, json_element_t* parent)
{
    this->type = type;
    vector_init(&this->children, 0, sizeof(json_element_t*));
    string_init(&this->name, name);
    this->parent = parent;
}

void json_element_deallocate(json_element_t* this)
{
    int i;
    json_element_t* child;

    string_deallocate(&this->name);

    for (i = 0; i < this->children.count; ++i)
    {
        child = VECTOR_GET_GENERIC(this->children, i, json_element_t*);
        json_element_deallocate(child);
    }

    vector_deallocate(&this->children);

    vfree(this);
}

void json_serializer_init(json_serializer_t* this)
{
    json_element_t* root;

    string_init(&this->buffer, "");

    root = json_serializer_add_element(this, NULL, "", JsonObject);
    this->root = root;
}

void json_serializer_deallocate(json_serializer_t* this)
{
    json_element_deallocate(this->root);

    string_deallocate(&this->buffer);
}

void json_serializer_add_tabs(json_serializer_t* this, int depth)
{
    int i;

    for (i = 0; i < depth; ++i)
    {
        string_append(&this->buffer, "  ");
    }
}

json_element_t* json_serializer_add_element(json_serializer_t* this, json_element_t* parent, char* name, json_type_t type)
{
    json_element_t* elem = vmalloc(sizeof(json_element_t));
    json_element_init(elem, name, type, parent);

    if (parent != NULL)
    {
        vector_push_back(&parent->children, &elem);
    }

    return elem;
}

json_element_t* json_serializer_add_object(json_serializer_t* this, json_element_t* parent, char* name)
{
    return json_serializer_add_element(this, parent, name, JsonObject);
}

json_element_t* json_serializer_add_array(json_serializer_t* this, json_element_t* parent, char* name)
{
    return json_serializer_add_element(this, parent, name, JsonArray);
}

json_element_t* json_serializer_add_value(json_serializer_t* this, json_element_t* parent, char* name, json_type_t type, void* value, int sizeof_T)
{
    json_element_t* elem;
    elem = json_serializer_add_element(this, parent, name, type);
    memcpy(elem->value, value, sizeof_T);

    return elem;
}

json_element_t* json_serializer_add_string(json_serializer_t* this, json_element_t* parent, char* name, char* value)
{
    return json_serializer_add_value(this, parent, name, JsonString, &value, sizeof(char*));
}

json_element_t* json_serializer_add_number(json_serializer_t* this, json_element_t* parent, char* name, long long value)
{
    return json_serializer_add_value(this, parent, name, JsonNumber, &value, sizeof(long long));
}

/*
json_element_t* json_serializer_add_real(json_serializer_t* this, json_element_t* parent, char* name, double value)
{
   return json_serializer_add_value(this, parent, name, JsonReal, &value, sizeof(double));
}
*/

json_element_t* json_serializer_add_bool(json_serializer_t* this, json_element_t* parent, char* name, bool value)
{
    return json_serializer_add_value(this, parent, name, JsonBool, &value, sizeof(bool));
}

json_element_t* json_serializer_add_null(json_serializer_t* this, json_element_t* parent, char* name)
{
    return json_serializer_add_element(this, parent, name, JsonNull);
}

static void to_string(json_serializer_t* this, json_element_t* elem, int depth, bool pretty)
{
    int i;
    json_element_t* child;
    char buf[64];

    if (pretty)
    {
        json_serializer_add_tabs(this, depth);
    }

    // Add element name, if it exists
    if (elem->name.vector.count > 0)
    {
        string_append(&this->buffer, "\"");
        string_append_str(&this->buffer, &elem->name);
        string_append(&this->buffer, "\": ");
    }

    switch (elem->type)
    {
    case JsonArray:
        string_append(&this->buffer, "[");
        if (pretty) string_append(&this->buffer, "\n");
        break;
    case JsonObject:
        string_append(&this->buffer, "{");
        if (pretty) string_append(&this->buffer, "\n");
        break;
    case JsonString:
        string_append(&this->buffer, "\"");
        string_append(&this->buffer, JSON_GET_GENERIC(elem, char*));
        string_append(&this->buffer, "\"");
        break;
    case JsonNumber:
        snprintf(buf, sizeof(buf), "%lld", JSON_GET_GENERIC(elem, long long));
        string_append(&this->buffer, buf);
        break;
    case JsonReal:
        // snprintf(buf, sizeof(buf), "%lf", JSON_GET_GENERIC(elem, double));
        string_append(&this->buffer, buf);
        break;
    case JsonBool:
        string_append(&this->buffer, JSON_GET_GENERIC(elem, bool) == true ? "true" : "false");
        break;
    case JsonNull:
        string_append(&this->buffer, "null");
        break;
    }

    // Recurse on all nested elements
    for (i = 0; i < elem->children.count; ++i)
    {
        child = VECTOR_GET_GENERIC(elem->children, i, json_element_t*);
        to_string(this, child, depth + 1, pretty);

        if (i < elem->children.count - 1)
        {
            string_append(&this->buffer, ",");
        }

        if (pretty)
        {
            string_append(&this->buffer, "\n");
        }
    }

    // Add closing grouping chars if neccessary
    switch (elem->type)
    {
    case JsonArray:
        if (pretty) json_serializer_add_tabs(this, depth);
        string_append(&this->buffer, "]");
        break;
    case JsonObject:
        if (pretty) json_serializer_add_tabs(this, depth);
        string_append(&this->buffer, "}");
        break;
    }
}

char* json_serializer_to_string(json_serializer_t* this, bool pretty)
{
    string_clear(&this->buffer);

    to_string(this, this->root, 0, pretty);

    if (pretty)
    {
        string_append(&this->buffer, "\n");
    }

    return string_c_str(&this->buffer);
}

int snprintf(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}