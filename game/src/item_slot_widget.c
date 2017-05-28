//  asprintf
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "item_slot_widget.h"
#include "gui_screen.h"
#include "item.h"
#include "panel.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static const int AttackIconTilesetId = 1043;
static const int DefendIconTilesetId = 1045;
static const int VitalityIconTilesetId = 1041;
static const int UnknownIconTilesetId = 1057;

static const char* EmptyString = "Empty";

static const int StatValuePadding = 4;
static const int StatValuePanelWidth = 24;

//  ---------------------------------------------------------------------------
static int GetStatIconTilesetId(enum StatType statType)
{
    switch (statType)
    {
        default:
            return UnknownIconTilesetId;
        case STAT_TYPE_ATTACK:
            return AttackIconTilesetId;
        case STAT_TYPE_DEFEND:
            return DefendIconTilesetId;
        case STAT_TYPE_VITALITY:
            return VitalityIconTilesetId;
    }
}

//  ---------------------------------------------------------------------------
struct ItemSlotWidget* CreateItemSlotWidget(struct GuiScreen* screen)
{
    //  Small panel to house the item's icon
    struct Panel* itemIconPanel = create_panel(NULL, PANEL_BORDER_STYLE_1);
    itemIconPanel->width = 24;
    itemIconPanel->height = 24;
    itemIconPanel->icon.style = PANEL_ICON_STYLE_NORMAL;
    AddGuiScreenPanel(screen, itemIconPanel);

    //  Panel for item name text
    struct Panel* itemNamePanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    itemNamePanel->width = 24;
    itemNamePanel->height = 14;
    AddGuiScreenPanel(screen, itemNamePanel);

    //  The ItemSlotWidget that will be returned to the caller
    struct ItemSlotWidget* itemSlotWidget = malloc(sizeof(struct ItemSlotWidget));
    itemSlotWidget->ItemIconPanel = itemIconPanel;
    itemSlotWidget->ItemNamePanel = itemNamePanel;

    //  Create the stat panels
    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        enum StatType statType = (enum StatType)s;

        int iconTilesetId = GetStatIconTilesetId(statType);

        //  Panel for small stat icon
        struct Panel* iconPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
        iconPanel->width = 8;
        iconPanel->height = 8;
        iconPanel->icon.style = PANEL_ICON_STYLE_SMALL;
        iconPanel->icon.tileset_id = iconTilesetId;
        iconPanel->visible = 0;
        AddGuiScreenPanel(screen, iconPanel);

        //  Panel for stat value text
        struct Panel* valuePanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
        valuePanel->width = StatValuePanelWidth;
        valuePanel->visible = 0;
        AddGuiScreenPanel(screen, valuePanel);

        itemSlotWidget->StatIconPanels[s] = iconPanel;
        itemSlotWidget->StatValuePanels[s] = valuePanel;
        itemSlotWidget->StatValues[s] = 0;
        itemSlotWidget->StatValueStrings[s] = NULL;
    }

    SetItemSlotWidgetPosition(itemSlotWidget, 0, 0);

    return itemSlotWidget;
}

//  ---------------------------------------------------------------------------
void DestroyItemSlotWidget(struct ItemSlotWidget** widget)
{
    assert(widget != NULL);

    if (*widget != NULL)
    {
        destroy_panel(&(*widget)->ItemIconPanel);
        destroy_panel(&(*widget)->ItemNamePanel);

        for (int p = 0; p < STAT_TYPE_COUNT; ++p)
        {
            destroy_panel(&(*widget)->StatIconPanels[p]);
            destroy_panel(&(*widget)->StatValuePanels[p]);

            if ((*widget)->StatValueStrings[p] != NULL)
            {
                free((*widget)->StatValueStrings[p]);
            }
        }

        free(*widget);
        *widget = NULL;
    }
}

//  ---------------------------------------------------------------------------
void SetItemSlotWidgetPosition(struct ItemSlotWidget* widget, int x, int y)
{
    widget->ItemIconPanel->X = x + 16;
    widget->ItemIconPanel->Y = y + 16;

    widget->ItemNamePanel->X = widget->ItemIconPanel->X + widget->ItemIconPanel->width + 4;
    widget->ItemNamePanel->Y = widget->ItemIconPanel->Y;

    int prevX = widget->ItemNamePanel->X;
    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        struct Panel* iconPanel = widget->StatIconPanels[s];
        iconPanel->X = prevX;
        iconPanel->Y = widget->ItemNamePanel->Y + widget->ItemNamePanel->height;

        struct Panel* valuePanel = widget->StatValuePanels[s];
        valuePanel->X = iconPanel->X + iconPanel->width + StatValuePadding;
        valuePanel->Y = iconPanel->Y;

        prevX = valuePanel->X + valuePanel->width;
    }
}

//  ---------------------------------------------------------------------------
void SetItemSlotWidgetVisible(struct ItemSlotWidget* widget, int visible)
{
    widget->ItemIconPanel->visible = visible;
    widget->ItemNamePanel->visible = visible;

    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        widget->StatIconPanels[s]->visible = visible;
        widget->StatValuePanels[s]->visible = visible;
    }
}

//  ---------------------------------------------------------------------------
void UpdateItemSlotWidget(struct ItemSlotWidget* widget, struct Item* item)
{
    if (item != NULL)
    {
        widget->ItemIconPanel->icon.tileset_id = item->tileset_id;
        widget->ItemNamePanel->text = item->name;
    }
    else
    {
        widget->ItemIconPanel->icon.tileset_id = -1;
        widget->ItemNamePanel->text = EmptyString;
    }

    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        enum StatType statType = (enum StatType)s;

        int statValue =
            item != NULL ?
            get_stats_value_by_stat_type(&item->stats, statType) :
            0;

        int* currentValue = &widget->StatValues[s];
        struct Panel* panel = widget->StatValuePanels[s];
        struct Panel* iconPanel = widget->StatIconPanels[s];
        char** stringValue = &widget->StatValueStrings[s];

        if (*currentValue != statValue)
        {
            *currentValue = statValue;

            if (*stringValue != NULL)
            {
                free(*stringValue);
                *stringValue = NULL;
            }

            if (*currentValue != 0)
            {
                asprintf(stringValue, "%d", *currentValue);
            }

            panel->text = *stringValue;
        }

        panel->visible = *currentValue != 0;
        iconPanel->visible = *currentValue != 0;
    }
}