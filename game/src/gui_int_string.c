//  asprintf
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "gui_int_string.h"
#include "gui_string.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

//  ---------------------------------------------------------------------------
struct GuiIntString* create_gui_int_string()
{
    struct GuiIntString* int_string = malloc(sizeof(struct GuiIntString));

    int_string->value = 0;
    int_string->label = NULL;
    int_string->string = NULL;

    return int_string;
}

//  ---------------------------------------------------------------------------
void destroy_gui_int_string(struct GuiIntString** int_string)
{
    assert(int_string != NULL);

    if (*int_string != NULL)
    {
        if ((*int_string)->label != NULL)
        {
            free((*int_string)->label);
        }

        if ((*int_string)->string != NULL)
        {
            free((*int_string)->string);
        }

        free(*int_string);
        *int_string = NULL;
    }
}

//  ---------------------------------------------------------------------------
void set_gui_int_string(
    struct GuiIntString* int_string,
    const char* label,
    int value)
{
    assert(int_string != NULL);

    if (int_string->value != value ||
        gui_string_compare(int_string->label, label) == 0)
    {
        int_string->value = value;

        if (int_string->label != NULL)
        {
            free(int_string->label);
            int_string->label = NULL;
        }

        if (int_string->string != NULL)
        {
            free(int_string->string);
            int_string->string = NULL;
        }

        if (label != NULL)
        {
            int_string->label = strdup(label);
            asprintf(&int_string->string, "%s: %d", label, value);
        }
        else
        {
            asprintf(&int_string->string, "%d", value);
        }
    }
}