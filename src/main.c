#include "rtti.h"
#include "string.h"
#include "vector.h"
#include <stdio.h>

void string_rtti_test()
{
    // Make an empty string
    Any str = Any$make_default(&type_string);

    // Append a value
    Any value = Any$from_cstr("Cool!");
    Any$invoke(str, "append", 1, &value);

    // Print it out
    puts(Any$invoke(str, "cstr", 0, NULL).value.cstr);

    // Prepend a value
    value = Any$from_cstr("You Are ");
    Any$invoke(str, "prepend", 1, &value);

    // Print it out
    puts(Any$invoke(str, "cstr", 0, NULL).value.cstr);

    // Free the str
    Any$free(&str);
}

void string_vector_test()
{
    Vector vec = Vector$new(&type_string);
    
    String str1 = String$from_cstr("Hello");
    Vector$push(&vec, &str1);

    String str2 = String$from_cstr("there");
    Vector$push(&vec, &str2);

    Vector$print(&vec, stdout); // Prints ["Hello", "there"]

    Vector$free(&vec);
}

int main(void)
{
    string_rtti_test();
    string_vector_test();

    // pause
    getc(stdin);
}
