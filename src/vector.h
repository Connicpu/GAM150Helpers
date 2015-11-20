////////////////////////////////////////////
// File    : string.h
// Author  : Connor Hilarides
// Created : 2015/11/19
////////////////////////////////////////////

#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct Vector Vector;
struct Type;

Vector Vector$new(const struct Type *member_type);
Vector Vector$copy(const Vector *vec);
void   Vector$free(Vector *this);

size_t Vector$len(const Vector *this);
void Vector$print(const Vector *this, FILE *stream);

void Vector$reserve(Vector *this, size_t cap);
void Vector$push(Vector *this, void *item);
void Vector$pop(Vector *this, void *result);

extern struct Type type_vector;
struct Vector
{
    const struct Type *member_type;
    void *data;
    size_t len;
    size_t cap;
};
