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

static const int ATTACK_ICON_TILESET_ID = 1043;
static const int DEFEND_ICON_TILESET_ID = 1045;
static const int VITALITY_ICON_TILESET_ID = 1041;
static const int UNKNOWN_ICON_TILESET_ID = 1057;

static const char* EMPTY_STRING = "Empty";

static const int STAT_VALUE_PANEL_PADDING = 4;
static const int STAT_VALUE_PANEL_WIDTH = 24;

//  ---------------------------------------------------------------------------
static int get_stat_icon_tileset_id(enum StatType stat_type)
{
    switch (stat_type)
    {
        default:
            return UNKNOWN_ICON_TILESET_ID;
        case STAT_TYPE_ATTACK:
            return ATTACK_ICON_TILESET_ID;
        case STAT_TYPE_DEFEND:
            return DEFEND_ICON_TILESET_ID;
        case STAT_TYPE_VITALITY:
            return VITALITY_ICON_TILESET_ID;
    }
}

//  ---------------------------------------------------------------------------
struct ItemSlotWidget* create_item_slot_widget(struct GuiScreen* gui_screen)
{
    //  Small panel to house the item's icon
    struct Panel* item_icon_panel = create_panel(NULL, PANEL_BORDER_STYLE_1);
    item_icon_panel->width = 24;
    item_icon_panel->height = 24;
    item_icon_panel->icon.style = PANEL_ICON_STYLE_NORMAL;
    add_panel_to_gui_screen(gui_screen, item_icon_panel);

    //  Panel for item name text
    struct Panel* item_name_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    item_name_panel->width = 24;
    item_name_panel->height = 14;
    add_panel_to_gui_screen(gui_screen, item_name_panel);

    //  The ItemSlotWidget that will be returned to the caller
    struct ItemSlotWidget* item_slot_widget = malloc(sizeof(struct ItemSlotWidget));
    item_slot_widget->item_icon_panel = item_icon_panel;
    item_slot_widget->item_name_panel = item_name_panel;

    //  Create the stat panels
    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        enum StatType stat_type = (enum StatType)s;

        int icon_tileset_id = get_stat_icon_tileset_id(stat_type);

        //  Panel for small stat icon
        struct Panel* icon_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
        icon_panel->width = 8;
        icon_panel->height = 8;
        icon_panel->icon.style = PANEL_ICON_STYLE_SMALL;
        icon_panel->icon.tileset_id = icon_tileset_id;
        icon_panel->visible = 0;
        add_panel_to_gui_screen(gui_screen, icon_panel);

        //  Panel for stat value text
        struct Panel* value_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
        value_panel->width = STAT_VALUE_PANEL_WIDTH;
        value_panel->visible = 0;
        add_panel_to_gui_screen(gui_screen, value_panel);

        item_slot_widget->stat_icon_panels[s] = icon_panel;
        item_slot_widget->stat_value_panels[s] = value_panel;
        item_slot_widget->stat_values[s] = 0;
        item_slot_widget->stat_value_strings[s] = NULL;
    }

    set_item_slot_widget_position(item_slot_widget, 0, 0);

    return item_slot_widget;
}

//  ---------------------------------------------------------------------------
void destroy_item_slot_widget(struct ItemSlotWidget** widget)
{
    assert(widget != NULL);

    if (*widget != NULL)
    {
        destroy_panel(&(*widget)->item_icon_panel);
        destroy_panel(&(*widget)->item_name_panel);

        for (int p = 0; p < STAT_TYPE_COUNT; ++p)
        {
            destroy_panel(&(*widget)->stat_icon_panels[p]);
            destroy_panel(&(*widget)->stat_value_panels[p]);

            if ((*widget)->stat_value_strings[p] != NULL)
            {
                free((*widget)->stat_value_strings[p]);
            }
        }

        free(*widget);
        *widget = NULL;
    }
}

//  ---------------------------------------------------------------------------
void set_item_slot_widget_position(struct ItemSlotWidget* widget, int x, int y)
{
    widget->item_icon_panel->x = x + 16;
    widget->item_icon_panel->y = y + 16;

    widget->item_name_panel->x = widget->item_icon_panel->x + widget->item_icon_panel->width + 4;
    widget->item_name_panel->y = widget->item_icon_panel->y;

    int prev_x = widget->item_name_panel->x;
    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        struct Panel* icon_panel = widget->stat_icon_panels[s];
        icon_panel->x = prev_x;
        icon_panel->y = widget->item_name_panel->y + widget->item_name_panel->height;

        struct Panel* value_panel = widget->stat_value_panels[s];
        value_panel->x = icon_panel->x + icon_panel->width + STAT_VALUE_PANEL_PADDING;
        value_panel->y = icon_panel->y;

        prev_x = value_panel->x + value_panel->width;
    }
}

//  ---------------------------------------------------------------------------
void set_item_slot_widget_visible(struct ItemSlotWidget* widget, int visible)
{
    widget->item_icon_panel->visible = visible;
    widget->item_name_panel->visible = visible;

    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        widget->stat_icon_panels[s]->visible = visible;
        widget->stat_value_panels[s]->visible = visible;
    }
}

//  ---------------------------------------------------------------------------
void update_item_slot_widget(struct ItemSlotWidget* widget, struct Item* item)
{
    if (item != NULL)
    {
        widget->item_icon_panel->icon.tileset_id = item->tileset_id;
        widget->item_name_panel->text = item->name;
    }
    else
    {
        widget->item_icon_panel->icon.tileset_id = -1;
        widget->item_name_panel->text = EMPTY_STRING;
    }

    for (int s = 0; s < STAT_TYPE_COUNT; ++s)
    {
        enum StatType stat_type = (enum StatType)s;

        int stat_value =
            item != NULL ?
            get_stats_value_by_stat_type(&item->stats, stat_type) :
            0;

        int* current_value = &widget->stat_values[s];
        struct Panel* panel = widget->stat_value_panels[s];
        struct Panel* icon_panel = widget->stat_icon_panels[s];
        char** string_value = &widget->stat_value_strings[s];

        if (*current_value != stat_value)
        {
            *current_value = stat_value;

            if (*string_value != NULL)
            {
                free(*string_value);
                *string_value = NULL;
            }

            if (*current_value != 0)
            {
                asprintf(string_value, "%d", *current_value);
            }

            panel->text = *string_value;
        }

        panel->visible = *current_value != 0;
        icon_panel->visible = *current_value != 0;
    }
}