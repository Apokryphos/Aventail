#ifndef ITEM_SLOT_WIDGET_HEADER_INCLUDED
#define ITEM_SLOT_WIDGET_HEADER_INCLUDED

#include "stat_type.h"

struct GuiScreen;
struct Item;
struct Panel;

struct ItemSlotWidget
{
    struct Panel* item_icon_panel;
    struct Panel* item_name_panel;
    struct Panel* stat_icon_panels[STAT_TYPE_COUNT];
    struct Panel* stat_value_panels[STAT_TYPE_COUNT];
    int stat_values[STAT_TYPE_COUNT];
    char* stat_value_strings[STAT_TYPE_COUNT];
};

struct ItemSlotWidget* create_item_slot_widget(struct GuiScreen* gui_screen);

void destroy_item_slot_widget(struct ItemSlotWidget** widget);

void set_item_slot_widget_position(struct ItemSlotWidget* widget, const int x, const int y);

void set_item_slot_widget_visible(struct ItemSlotWidget* widget, const int visible);

void update_item_slot_widget(struct ItemSlotWidget* widget, const struct Item* item);

#endif