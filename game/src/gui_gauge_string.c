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
struct GuiGaugeString* CreateGuiGaugeString()
{
    struct GuiGaugeString* gaugeString = malloc(sizeof(struct GuiGaugeString));

    gaugeString->Value = 0;
    gaugeString->MaxValue = 0;
    gaugeString->Label = NULL;
    gaugeString->String = NULL;

    return gaugeString;
}

//  ---------------------------------------------------------------------------
void DestroyGuiGaugeString(struct GuiGaugeString** gaugeString)
{
    assert(gaugeString != NULL);

    if (*gaugeString != NULL)
    {
        if ((*gaugeString)->Label != NULL)
        {
            free((*gaugeString)->Label);
        }

        if ((*gaugeString)->String != NULL)
        {
            free((*gaugeString)->String);
        }

        free(*gaugeString);
        *gaugeString = NULL;
    }
}

//  ---------------------------------------------------------------------------
void SetGuiGaugeString(
    struct GuiGaugeString* gaugeString,
    const char* label,
    int value,
    int maxValue)
{
    assert(gaugeString != NULL);

    if (gaugeString->Value != value ||
        gaugeString->MaxValue != maxValue ||
        GuiStringMatches(gaugeString->Label, label) == 0)
    {
        gaugeString->Value = value;
        gaugeString->MaxValue = maxValue;

        if (gaugeString->Label != NULL)
        {
            free(gaugeString->Label);
            gaugeString->Label = NULL;
        }

        if (gaugeString->String != NULL)
        {
            free(gaugeString->String);
            gaugeString->String = NULL;
        }

        if (label != NULL)
        {
            gaugeString->Label = strdup(label);
            asprintf(&gaugeString->String, "%s: %d / %d", label, value, maxValue);
        }
        else
        {
            asprintf(&gaugeString->String, "%d / %d", value, maxValue);
        }
    }
}