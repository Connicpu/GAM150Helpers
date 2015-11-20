#include "rtti.h"
#include "string.h"
#include <stdio.h>

int main(void)
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

    // pause
    getc(stdin);
}
