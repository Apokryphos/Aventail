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
    struct Panel* itemIconPanel = CreatePanel(NULL, PANEL_BORDER_STYLE_1);
    itemIconPanel->Width = 24;
    itemIconPanel->Height = 24;
    itemIconPanel->Icon.Style = PANEL_ICON_STYLE_NORMAL;
    AddGuiScreenPanel(screen, itemIconPanel);

    //  Panel for item name text
    struct Panel* itemNamePanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    itemNamePanel->Width = 24;
    itemNamePanel->Height = 14;
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
        struct Panel* iconPanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
        iconPanel->Width = 8;
        iconPanel->Height = 8;
        iconPanel->Icon.Style = PANEL_ICON_STYLE_SMALL;
        iconPanel->Icon.TilesetId = iconTilesetId;
        iconPanel->Visible = 0;
        AddGuiScreenPanel(screen, iconPanel);

        //  Panel for stat value text
        struct Panel* valuePanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
        valuePanel->Width = StatValuePanelWidth;
        valuePanel->Visible = 0;
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
        DestroyPanel(&(*widget)->ItemIconPanel);
        DestroyPanel(&(*widget)->ItemNamePanel);

        for (int p = 0; p < STAT_TYPE_COUNT; ++p)
        {
            DestroyPanel(&(*widget)->StatIconPanels[p]);
            DestroyPanel(&(*widget)->StatValuePanels[p]);

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

    widget->ItemNamePanel->X = widget->ItemIconPanel->X + widget->ItemIconPanel->Width + 4;
    widget->ItemNamePanel->Y = widget->ItemIconPanel->Y;

    int prevX = widget->ItemNamePanel->X;
    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        struct Panel* iconPanel = widget->StatIconPanels[s];
        iconPanel->X = prevX;
        iconPanel->Y = widget->ItemNamePanel->Y + widget->ItemNamePanel->Height;

        struct Panel* valuePanel = widget->StatValuePanels[s];
        valuePanel->X = iconPanel->X + iconPanel->Width + StatValuePadding;
        valuePanel->Y = iconPanel->Y;

        prevX = valuePanel->X + valuePanel->Width;
    }
}

//  ---------------------------------------------------------------------------
void SetItemSlotWidgetVisible(struct ItemSlotWidget* widget, int visible)
{
    widget->ItemIconPanel->Visible = visible;
    widget->ItemNamePanel->Visible = visible;

    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        widget->StatIconPanels[s]->Visible = visible;
        widget->StatValuePanels[s]->Visible = visible;
    }
}

//  ---------------------------------------------------------------------------
void UpdateItemSlotWidget(struct ItemSlotWidget* widget, struct Item* item)
{
    if (item != NULL)
    {
        widget->ItemIconPanel->Icon.TilesetId = item->tileset_id;
        widget->ItemNamePanel->Text = item->name;
    }
    else
    {
        widget->ItemIconPanel->Icon.TilesetId = -1;
        widget->ItemNamePanel->Text = EmptyString;
    }

    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        enum StatType statType = (enum StatType)s;

        int statValue =
            item != NULL ?
            GetStatByType(&item->stats, statType) :
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

            panel->Text = *stringValue;
        }

        panel->Visible = *currentValue != 0;
        iconPanel->Visible = *currentValue != 0;
    }
}