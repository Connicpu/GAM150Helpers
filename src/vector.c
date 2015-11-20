#include "vector.h"
#include "rtti.h"
#include <string.h>

static void *Vector$mem_idx(const Vector *this, size_t idx);
static void Vector$grow(Vector *this, size_t minimum);

Vector Vector$new(const Type *member_type)
{
    Vector vec = { NULL };
    vec.member_type = member_type;
    return vec;
}

Vector Vector$copy(const Vector *vec)
{
    Vector copy = Vector$new(vec->member_type);
    Vector$reserve(&copy, vec->len);
    for (size_t i = 0; i < vec->len; ++i)
    {
        Any obj = Any$ref_complex(vec->member_type, Vector$mem_idx(vec, i));
        Any item = Any$copy(obj);
        Vector$push(&copy, item.value.ptr);
        Any$soft_release(&item);
    }
    return copy;
}

void Vector$free(Vector *this)
{
    for (size_t i = 0; i < this->len; ++i)
    {
        Any obj = Any$ref_complex(this->member_type, Vector$mem_idx(this, i));
        Any$delete_ref(&obj);
    }

    free(this->data);
    this->data = NULL;
    this->len = 0;
    this->cap = 0;
}

size_t Vector$len(const Vector *this)
{
    return this->len;
}

void Vector$print(const Vector *this, FILE *stream)
{
    fputs("[", stream);

    for (size_t i = 0; i < this->len; ++i)
    {
        Any obj = Any$ref_complex(this->member_type, Vector$mem_idx(this, i));
        Any$print(obj, stream);
        if (i + 1 < this->len) { fputs(", ", stream); }
    }

    fputs("]\n", stream);
}

void Vector$reserve(Vector *this, size_t cap)
{
    if (this->cap < cap)
    {
        Vector$grow(this, cap);
    }
}

void Vector$push(Vector *this, void *item)
{
    Vector$reserve(this, this->len + 1);
    memcpy(Vector$mem_idx(this, this->len++), item, this->member_type->size);
}

void Vector$pop(Vector *this, void *result)
{
    memcpy(result, Vector$mem_idx(this, --this->len), this->member_type->size);
}

static void *Vector$mem_idx(const Vector *this, size_t idx)
{
    return &((char *)this->data)[idx * this->member_type->size];
}

void Vector$grow(Vector *this, size_t minimum)
{
    if (!minimum || minimum < this->cap)
    {
        return;
    }

    size_t new_cap = this->cap * 2;
    if (new_cap < minimum)
    {
        new_cap = minimum;
    }

    size_t new_size = new_cap * this->member_type->size;
    if (this->data)
    {
        this->data = realloc(this->data, new_size);
    }
    else
    {
        this->data = malloc(new_size);
    }
}
