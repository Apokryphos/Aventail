#ifndef STRING_UTIL_HEADER_INCLUDED
#define STRING_UTIL_HEADER_INCLUDED

#include <stddef.h>

/*
    Finds tokens in a comma-separated string.
    Ignores empty tokens and trailing and leading whitespace.
    Allocated tokens should be freed by the caller.
    Returns the number of tokens found and allocated.
*/
size_t Tokenize(char* str, char** tokens[]);

#endif