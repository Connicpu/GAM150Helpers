////////////////////////////////////////////
// File    : string.c
// Author  : Connor Hilarides
// Created : 2015/11/19
////////////////////////////////////////////

#include "string.h"
#include "rtti.h"
#include "helpers.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static size_t String$extra_cap(const String *this);
static void String$grow(String *this, size_t minimum);

struct String String$EMPTY = { "", 0, 0 };

String String$new()
{
    return String$EMPTY;
}

String String$copy(const String *this)
{
    String copy = String$EMPTY;
    String$append(&copy, *this);
    return copy;
}

void String$free(String *this)
{
    if (this->cap)
    {
        free(this->data);
    }

    *this = String$EMPTY;
}

String String$from_cstr(const char *str)
{
    String ref = String$from_literal(str);
    return String$copy(&ref);
}

String String$from_literal(const char *lit)
{
    String ref;
    ref.data = (char *)lit; // _cap=0 guarantees _data will be treated as const
    ref.len = (unsigned)strlen(lit);
    ref.cap = 0;
    return ref;
}

const char *String$cstr(const String *this)
{
    return this->data;
}

size_t String$len(const String *this)
{
    return this->len;
}

bool String$equal(String lhs, String rhs)
{
    return String$compare(lhs, rhs) == 0;
}

int String$compare(String lhs, String rhs)
{
    return strcmp(lhs.data, rhs.data);
}

void String$reserve(String *this, size_t minimum)
{
    // If our capacity isn't enough for minimum plus
    // a NUL terminator, grow it
    if (this->cap <= minimum + 1)
    {
        String$grow(this, minimum);
    }
}

void String$append(String *this, String rhs)
{
    // Ensure the string can fit [this.., rhs.., '\0']
    String$reserve(this, this->len + rhs.len);
    // Copy rhs into our data
    memcpy(this->data + this->len, rhs.data, rhs.len);
    // Increment the length
    this->len += rhs.len;
    // Apply the NUL terminator
    this->data[this->len] = 0;
}

void String$prepend(String *this, String rhs)
{
    // Ensure the string can fit [rhs.., this.., '\0']
    String$reserve(this, this->len + rhs.len);
    // Shift the string over
    memmove(this->data + rhs.len, this->data, this->len);
    // Copy the new prefix in
    memcpy(this->data, rhs.data, rhs.len);
    // Increment the length
    this->len += rhs.len;
    // Apply the NUL terminator
    this->data[this->len] = 0;
}

void String$push(String *this, char c)
{
    // Ensure there is room to push a character
    String$reserve(this, this->len + 1);
    // Push a character
    this->data[this->len++] = c;
    // Push the NUL terminator
    this->data[this->len] = 0;
}

void String$pop(String *this)
{
    // There is no popping to do if the string is empty
    if (!this->len)
    {
        return;
    }

    // Make sure we own the string
    if (!this->cap)
    {
        String$reserve(this, this->len);
    }

    // Pop a character
    this->data[--this->len] = 0;
}

static size_t String$extra_cap(const String *this)
{
    if (this->cap)
    {
        // Subtract 1 to maintain the cstr property
        return this->cap - this->len - 1;
    }
    else
    {
        return 0;
    }
}

static void String$grow(String *this, size_t minimum)
{
    size_t new_size;

    // We shouldn't do anything if they're asking for nothing
    if (minimum == 0)
    {
        return;
    }

    // Ensure this new minimum isn't less than the string
    // we're already storing
    if (minimum < this->len)
    {
        minimum = this->len;
    }

    // Try to double the size, but if that isn't
    // big enough just set it to the minumum required
    if (this->cap * 2 >= minimum + 1)
    {
        new_size = this->cap * 2;
    }
    else
    {
        new_size = minimum + 1;
    }

    // If the string has already been allocated by us,
    // we can just use realloc. Otherwise, we have to malloc
    // new space and copy the string we didn't own in.
    if (this->cap)
    {
        char *temp = realloc(this->data, new_size);
        assert(temp && "Uh oh, memory allocation failed");
        this->data = temp;
    }
    else
    {
        char *temp = malloc(new_size);
        assert(temp && "Uh oh, memory allocation failed");
        memcpy(temp, this->data, this->len);
        temp[this->len] = 0;
        this->data = temp;
    }
    this->cap = new_size;
}

///////////////////////////////////////////////
// String RTTI

static Any rtti_constructor(void *obj, unsigned arg_count, Any *arguments)
{
    (obj); // unreferenced parameter

    Any result;
    switch (arg_count)
    {
        case 0: // Default constructor
        {
            result = Any$from_complex(&type_string, &String$EMPTY);
            break;
        }
        case 1: // 1-arg constructor
        {
            if (arguments[0].type == &type_cstr) // Create from a cstr
            {
                const char *cstr = arguments[0].value.cstr;
                String temp = String$from_cstr(cstr);
                result = Any$from_complex(&type_string, &temp);
                break;
            }
            else if (arguments[0].type == &type_string) // Copy constructor
            {
                String temp, copied;
                Any$unpack(arguments[0], &temp);
                Any$soft_release(&arguments[0]);

                copied = String$copy(&temp);
                result = Any$from_complex(&type_string, &temp);
                break;
            }
            else if (arguments[0].type == &type_string_ptr) // Reference to a string
            {
                String temp;
                const String *ptr = (const String *)arguments[0].value.ptr;
                if (ptr)
                {
                    temp = String$copy(ptr);
                }
                else
                {
                    temp = String$EMPTY;
                }
                result = Any$from_complex(&type_string, &temp);
                break;
            }
            else
            {
                result = Any$EMPTY;
                break;
            }
        }
        default: // Invalid
        {
            result = Any$EMPTY;
            break;
        }
    }

    return result;
}

static Member constructor_member =
{
    ".ctor",
    rtti_constructor,
    1, // Maximum number of args
    NULL, // Argument types; function is overloaded
    &type_string, // Return type
    true, // static
    true, // overloaded
};

static Any rtti_destructor(void *obj, unsigned arg_count, Any *arguments)
{
    (arguments); // unreferenced parameter
    assert(arg_count == 0);
    String$free((String *)obj);
    return Any$VOID;
}

static Member destructor_member =
{
    ".dtor",
    rtti_destructor,
    0, // Number of args
    NULL, // No arguments
    &type_void, // Return type
    false, // static
    false, // overloaded
};

static Any rtti_cstr(void *obj, unsigned arg_count, Any *arguments)
{
    (arg_count, arguments); // unreferenced parameters
    return Any$from_cstr(String$cstr((const String *)obj));
}

static Member cstr_member =
{
    "cstr",
    rtti_cstr,
    0, NULL, // Args
    &type_cstr, // Return type
    false, // static
    false, // overloaded
};

static Any rtti_len(void *obj, unsigned arg_count, Any *arguments)
{
    (arg_count, arguments); // unreferenced parameters
    return Any$from_size_t(String$len((const String *)obj));
}

static Member len_member =
{
    "len",
    rtti_len,
    0, NULL, // Args
    &type_size_t, // Return type
    false, // static
    false, // overloaded
};

static Any rtti_append(void *obj, unsigned arg_count, Any *arguments)
{
    assert(arg_count == 1);

    String *this = (String *)obj;
    if (arguments[0].type == &type_string || arguments[1].type == &type_string_ptr)
    {
        String *rhs = (String *)arguments[0].value.ptr;
        String$append(this, *rhs);
    }
    else if (arguments[0].type == &type_cstr)
    {
        String$append(this, String$from_literal(arguments[0].value.cstr));
    }
    else
    {
        assert(false && "Invalid type passed to String$append");
    }

    return Any$VOID;
}

static Type *append_args[] =
{
    &type_string
};

static Member append_member =
{
    "append",
    rtti_append,
    ARRAY_SIZE(append_args), append_args, // Args
    &type_void, // Return type
    false, // static
    false, // overloaded
};

static Any rtti_prepend(void *obj, unsigned arg_count, Any *arguments)
{
    assert(arg_count == 1);

    String *this = (String *)obj;
    if (arguments[0].type == &type_string || arguments[1].type == &type_string_ptr)
    {
        String *rhs = (String *)arguments[0].value.ptr;
        String$prepend(this, *rhs);
    }
    else if (arguments[0].type == &type_cstr)
    {
        String$prepend(this, String$from_literal(arguments[0].value.cstr));
    }
    else
    {
        assert(false && "Invalid type passed to String$append");
    }

    return Any$VOID;
}

static Type *prepend_args[] =
{
    &type_string
};

static Member prepend_member =
{
    "prepend",
    rtti_prepend,
    ARRAY_SIZE(prepend_args), prepend_args, // Args
    &type_void, // Return type
    false, // static
    false, // overloaded
};

static Member *member_list[] =
{
    &constructor_member,
    &destructor_member,
    &cstr_member,
    &len_member,
    &append_member,
    &prepend_member,
};

struct Type type_string =
{
    TK_COMPLEX,
    sizeof(String), // Size
    sizeof(char *), // Alignment
    "String", // Name
    NULL, // Subtype
    &constructor_member,
    &destructor_member,

    0, NULL, // No accessible fields
    ARRAY_SIZE(member_list), member_list,
    0, NULL, // String does not implement any interfaces
};

struct Type type_string_ptr =
{
    TK_POINTER,
    sizeof(String *),
    sizeof(String *),
    "String*",
    &type_string,
};
