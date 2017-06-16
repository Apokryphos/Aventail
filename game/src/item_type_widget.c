#include "audio.h"
#include "direction.h"
#include "gui_screen.h"
#include "input_device.h"
#include "item_type_widget.h"
#include "panel.h"
#include <stdlib.h>

//  ---------------------------------------------------------------------------
static void select_item_type(
    struct ItemTypeWidget* widget,
    enum ItemType item_type)
{
    widget->item_type_panels[(int)widget->selected_item_type]->show_title = 0;

    play_sfx(SFX_MENU_NAV);
    widget->selected_item_type = item_type;
    widget->item_type_panels[(int)item_type]->show_title = 1;
}

//  ---------------------------------------------------------------------------
static void select_next_item_type(struct ItemTypeWidget* widget)
{
    int t = (int)widget->selected_item_type;

    if (t < ITEM_TYPE_COUNT - 1)
    {
        ++t;
    }
    else
    {
        t = 0;
    }

    select_item_type(widget, (enum ItemType)t);
}

//  ---------------------------------------------------------------------------
static void select_previous_item_type(struct ItemTypeWidget* widget)
{
    int t = (int)widget->selected_item_type;

    if (t > 0)
    {
        --t;
    }
    else
    {
        t = ITEM_TYPE_COUNT - 1;
    }

    select_item_type(widget, (enum ItemType)t);
}

//  ---------------------------------------------------------------------------
static int get_item_type_icon_tileset(enum ItemType item_type)
{
    switch (item_type)
    {
        default:
            return 248;
        case ITEM_TYPE_NONE:
            return 91;
        case ITEM_TYPE_CONSUMABLE:
            return 98;
        case ITEM_TYPE_WEAPON:
            return 137;
        case ITEM_TYPE_ARMOR:
            return 166;
        case ITEM_TYPE_SHIELD:
            return 176;
        case ITEM_TYPE_ACCESSORY:
            return 92;
    }
}

//  ---------------------------------------------------------------------------
static struct Panel* create_item_type_panel(enum ItemType item_type)
{
    struct Panel* panel = create_panel(
        get_item_type_category_string(item_type),
        PANEL_BORDER_STYLE_1);

    panel->width = 24;
    panel->height = 24;
    panel->background = 1;
    panel->icon.style = PANEL_ICON_STYLE_NORMAL;
    panel->icon.tileset_id = get_item_type_icon_tileset(item_type);
    panel->show_title = 0;

    return panel;
}

//  ---------------------------------------------------------------------------
struct ItemTypeWidget* create_item_type_widget(struct GuiScreen* gui_screen)
{
    struct ItemTypeWidget* widget = malloc(sizeof(struct ItemTypeWidget));

    widget->selected_item_type = ITEM_TYPE_NONE;

    for (int t = 0; t < ITEM_TYPE_COUNT; ++t)
    {
        struct Panel* panel = create_item_type_panel((enum ItemType)t);
        widget->item_type_panels[t] = panel;
        add_panel_to_gui_screen(gui_screen, panel);
    }

    widget->item_type_panels[0]->show_title = 1;

    set_item_type_widget_position(widget, 0, 0);

    return widget;
}

//  ---------------------------------------------------------------------------
struct Panel* get_selected_item_type_panel(struct ItemTypeWidget* widget)
{
    return widget->item_type_panels[(int)widget->selected_item_type];
}

//  ---------------------------------------------------------------------------
void process_item_type_widget_input(
    struct ItemTypeWidget* widget,
    struct InputDevice* input_device)
{
    if (input_device->move_direction == DIRECTION_RIGHT)
    {
        select_next_item_type(widget);
    }
    else if (input_device->move_direction == DIRECTION_LEFT)
    {
        select_previous_item_type(widget);
    }
}

//  ---------------------------------------------------------------------------
void reset_item_type_widget(struct ItemTypeWidget* widget)
{
    get_selected_item_type_panel(widget)->show_title = 0;
    widget->selected_item_type = ITEM_TYPE_NONE;
    widget->item_type_panels[0]->show_title = 1;
}

//  ---------------------------------------------------------------------------
void set_item_type_widget_position(
    struct ItemTypeWidget* widget,
    const int x,
    const int y)
{
    for (int t = 0; t < ITEM_TYPE_COUNT; ++t)
    {
        struct Panel* panel = widget->item_type_panels[t];
        panel->x = x + (t * panel->width);
        panel->y = y;
    }
}