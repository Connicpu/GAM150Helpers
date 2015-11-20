////////////////////////////////////////////
// File    : rtti.h
// Author  : Connor Hilarides
// Created : 2015/11/19
////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/////////////////////////////////////
// Type declarations
typedef enum TypeKind TypeKind;
typedef struct Type Type;
typedef struct Interface Interface;
typedef struct Field Field;
typedef struct Member Member;
typedef struct Any Any;

/////////////////////////////////////
// Type manipulation functions
const Field *Type$find_field(const Type *this, const char *name);
const Member *Type$find_member(const Type *this, const char *name);

const Any Member$invoke(const Member *this, void *obj, unsigned arg_count, Any *args);

Any Any$make_default(const Type *type);
Any Any$from_int8(int8_t i);
Any Any$from_uint8(uint8_t u);
Any Any$from_int16(int16_t i);
Any Any$from_uint16(uint16_t u);
Any Any$from_int32(int32_t i);
Any Any$from_uint32(uint32_t u);
Any Any$from_int64(int64_t i);
Any Any$from_uint64(uint64_t u);
Any Any$from_size_t(size_t s);
Any Any$from_float(float f);
Any Any$from_double(double d);
// You need to make sure the string will live as long as the Any value is needed
// Construct a String value if you are unsure
Any Any$from_cstr(const char *cstr);
// Note that this function takes ownership of the value
Any Any$from_complex(const Type *type, void *value);
Any Any$ref_complex(const Type *type, void *value);
Any Any$copy(Any obj);
void Any$unpack(Any boxed, void *placement);
void Any$free(Any *boxed);
void Any$freev(Any boxed);
void Any$soft_release(Any *boxed);
void Any$delete_ref(Any *boxed);
Any Any$invoke(Any self, const char *member_name, unsigned arg_count, Any *args);
void Any$print(Any obj, FILE *stream);

/////////////////////////////////////
// Type types
enum TypeKind
{
    TK_VOID,
    TK_PRIMITIVE,
    TK_POINTER,
    TK_COMPLEX,
};

struct Type
{
    TypeKind kind;
    unsigned size;
    unsigned alignment;
    const char *name;
    const Type *subtype;

    const Member *constructor;
    const Member *destructor;

    unsigned field_count;
    const Field **fields;

    unsigned member_count;
    const Member **members;

    unsigned interface_count;
    const Interface **interfaces;
};

struct Interface
{
    const char *name;
    unsigned member_count;
    const Member **members;
};

struct Field
{
    const char *name;
    const Type *type;
    unsigned struct_offset;
    bool is_pointer;
};

struct Member
{
    const char *name;

    Any(*invoke)(void *obj, unsigned arg_count, Any *arguments);

    unsigned argument_count; // Maximum args if overloaded.
    const Type **argument_types; // NULL is valid if there is no 1 set of correct args
    const Type *return_type; // Return type of the function
    bool is_static; // Whether obj needs to be set to an instance
    bool is_overloaded; // Whether fewer args than the maximum can be given
};

union AnyData
{
    int8_t i8;
    uint8_t u8;
    int16_t i16;
    uint16_t u16;
    int32_t i32;
    uint32_t u32;
    int64_t i64;
    uint64_t u64;
    float f32;
    double f64;
    void *ptr;
    const char *cstr;
};

struct Any
{
    const Type *type;
    union AnyData value;
};

/////////////////////////////////////
// Primitive types
extern Type type_void;
extern Type type_int8_t;
extern Type type_uint8_t;
extern Type type_int16_t;
extern Type type_uint16_t;
extern Type type_int32_t;
extern Type type_uint32_t;
extern Type type_int64_t;
extern Type type_uint64_t;
extern Type type_size_t;
extern Type type_float;
extern Type type_double;
extern Type type_cstr;
extern Type type_any;

/////////////////////////////////////
// Other statics
extern Any Any$EMPTY;
extern Any Any$VOID;
