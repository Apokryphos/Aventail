#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const size_t TOKEN_BUFFER_SIZE = 512;

//  ---------------------------------------------------------------------------
size_t Tokenize(const char* str, char** tokens[])
{
    //  Specified string is NULL
    if (str == NULL)
    {
        *tokens = NULL;
        return 0;
    }

    //  Specified string is length zero
    size_t strLen = strlen(str);
    if (strLen == 0)
    {
        *tokens = NULL;
        return 0;      
    }

    char* buffer[TOKEN_BUFFER_SIZE];
    size_t b = 0;

    const char* start = str;

    while (start < str + strLen)
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
            end = &str[strLen];
        }

        //  Ignore trailing whitespace
        const char* lastChar = end;
        while ((*lastChar == ',' ||
                 isspace(*lastChar) || 
                 *lastChar == '\0') && lastChar > start)
        {
            --lastChar;
        }

        if (lastChar != end)
        {
            end = lastChar + 1;
        }

        size_t tokenLen = end - start;

        if (tokenLen > 0)
        {
            assert(b < TOKEN_BUFFER_SIZE);

            //  Allocate token string
            buffer[b] = malloc(sizeof(char) * tokenLen + 1);
            strncpy(buffer[b], start, tokenLen);
            buffer[b][tokenLen] = '\0';
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