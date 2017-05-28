#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
int gui_string_compare(const char* guiString, const char* value)
{
    return
        ((guiString == NULL && value == NULL) ||
        (guiString != NULL && value != NULL && strcmp(guiString, value) == 0));
}

//  ---------------------------------------------------------------------------
void set_gui_string(char **string, const char* value)
{
    assert(string != NULL);

    if (gui_string_compare(*string, value) == 0)
    {
        if (*string != NULL)
        {
            free(*string);
            *string = NULL;
        }

        if (value != NULL)
        {
            *string = strdup(value);
        }
    }
}