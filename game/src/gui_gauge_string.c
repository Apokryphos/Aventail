//  asprintf
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "gui_gauge_string.h"
#include "gui_string.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

//  ---------------------------------------------------------------------------
struct GuiGaugeString* create_gui_gauge_string()
{
    struct GuiGaugeString* gauge_string = malloc(sizeof(struct GuiGaugeString));

    gauge_string->value = 0;
    gauge_string->max_value = 0;
    gauge_string->label = NULL;
    gauge_string->string = NULL;

    return gauge_string;
}

//  ---------------------------------------------------------------------------
void destroy_gui_gauge_string(struct GuiGaugeString** gauge_string)
{
    assert(gauge_string != NULL);

    if (*gauge_string != NULL)
    {
        if ((*gauge_string)->label != NULL)
        {
            free((*gauge_string)->label);
        }

        if ((*gauge_string)->string != NULL)
        {
            free((*gauge_string)->string);
        }

        free(*gauge_string);
        *gauge_string = NULL;
    }
}

//  ---------------------------------------------------------------------------
void set_gui_gauge_string(
    struct GuiGaugeString* gauge_string,
    const char* label,
    int value,
    int max_value)
{
    assert(gauge_string != NULL);

    if (gauge_string->value != value ||
        gauge_string->max_value != max_value ||
        gui_string_compare(gauge_string->label, label) == 0)
    {
        gauge_string->value = value;
        gauge_string->max_value = max_value;

        if (gauge_string->label != NULL)
        {
            free(gauge_string->label);
            gauge_string->label = NULL;
        }

        if (gauge_string->string != NULL)
        {
            free(gauge_string->string);
            gauge_string->string = NULL;
        }

        if (label != NULL)
        {
            gauge_string->label = strdup(label);
            asprintf(&gauge_string->string, "%s: %d / %d", label, value, max_value);
        }
        else
        {
            asprintf(&gauge_string->string, "%d / %d", value, max_value);
        }
    }
}