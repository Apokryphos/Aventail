#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const size_t TOKEN_BUFFER_SIZE = 512;

//  ---------------------------------------------------------------------------
size_t tokenize_string(const char* str, char** tokens[])
{
    //  Specified string is NULL
    if (str == NULL)
    {
        *tokens = NULL;
        return 0;
    }

    //  Specified string is length zero
    size_t str_len = strlen(str);
    if (str_len == 0)
    {
        *tokens = NULL;
        return 0;
    }

    char* buffer[TOKEN_BUFFER_SIZE];
    size_t b = 0;

    const char* start = str;

    while (start < str + str_len)
    {
        //  Ignore leading whitespace
        while (isspace(*start))
        {
            ++start;
        }

        //  Ignore if token is all whitespace
        if (*start == '\0')
        {
            break;
        }

        const char* end = strchr(start, ',');
        if (end == NULL)
        {
            end = &str[str_len];
        }

        //  Ignore trailing whitespace
        const char* last_char = end;
        while ((*last_char == ',' ||
                 isspace(*last_char) ||
                 *last_char == '\0') && last_char > start)
        {
            --last_char;
        }

        if (last_char != end)
        {
            end = last_char + 1;
        }

        size_t token_len = end - start;

        if (token_len > 0)
        {
            assert(b < TOKEN_BUFFER_SIZE);

            //  Allocate token string
            buffer[b] = malloc(sizeof(char) * token_len + 1);
            strncpy(buffer[b], start, token_len);
            buffer[b][token_len] = '\0';
            ++b;
        }

        start = end + 1;
    }

    //  Allocate array of pointers to store token strings
    if (b > 0)
    {
        (*tokens) = malloc(sizeof(char*) * b);
        for (size_t t = 0; t < b; ++t)
        {
            (*tokens)[t] = buffer[t];
        }
        return b;
    }

    //  No tokens found
    *tokens = NULL;
    return 0;
}