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
struct GuiIntString* CreateGuiIntString()
{
    struct GuiIntString* intString = malloc(sizeof(struct GuiIntString));

    intString->Value = 0;
    intString->Label = NULL;
    intString->String = NULL;

    return intString;
}

//  ---------------------------------------------------------------------------
void DestroyGuiIntString(struct GuiIntString** intString)
{
    assert(intString != NULL);

    if (*intString != NULL)
    {
        if ((*intString)->Label != NULL)
        {
            free((*intString)->Label);
        }

        if ((*intString)->String != NULL)
        {
            free((*intString)->String);
        }

        free(*intString);
        *intString = NULL;
    }
}



//  ---------------------------------------------------------------------------
void SetGuiIntString(
    struct GuiIntString* intString,
    const char* label,
    int value)
{
    assert(intString != NULL);

    if (intString->Value != value ||
        GuiStringMatches(intString->Label, label) == 0)
    {
        intString->Value = value;

        if (intString->Label != NULL)
        {
            free(intString->Label);
            intString->Label = NULL;
        }

        if (intString->String != NULL)
        {
            free(intString->String);
            intString->String = NULL;
        }

        if (label != NULL)
        {
            intString->Label = strdup(label);
            asprintf(&intString->String, "%s: %d", label, value);
        }
        else
        {
            asprintf(&intString->String, "%d", value);
        }
    }
}