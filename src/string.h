////////////////////////////////////////////
// File    : string.h
// Author  : Connor Hilarides
// Created : 2015/11/19
////////////////////////////////////////////

#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct String String;
extern struct String String$EMPTY;

// Construct a String from a "String Literal"
#define STR(cstr) (String$from_literal(cstr""))

String String$new();
String String$copy(const String *this);
void   String$free(String *this);

String      String$from_cstr(const char *str);
String      String$from_literal(const char *lit);
const char *String$cstr(const String *this);
size_t      String$len(const String *this);
bool        String$equal(String lhs, String rhs);
int         String$compare(String lhs, String rhs);

void String$reserve(String *this, size_t minimum);
void String$append(String *this, String rhs);
void String$prepend(String *this, String rhs);
void String$push(String *this, char c);
void String$pop(String *this);

extern struct Type type_string;
extern struct Type type_string_ptr;
struct String
{
    char *_data;
    size_t _len;
    size_t _cap;
};
