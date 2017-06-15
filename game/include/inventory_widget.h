#ifndef INVENTORY_WIDGET_HEADER_INCLUDED
#define INVENTORY_WIDGET_HEADER_INCLUDED

#include "item_type.h"
#include <stddef.h>

struct GuiScreen;
struct InputDevice;
struct Inventory;
struct Item;
struct ItemSlotWidget;
struct Panel;

#define INVENTORY_WIDGET_MAX_VISIBLE_ITEMS 5
struct InventoryWidget
{
    /*
        The ItemType used as a filter when populating the Items member.
    */
    enum ItemType item_type;
    /*
        The number of items in the Items member.
    */
    size_t item_count;
    /*
        The index into the Items member of the currently selected item.
    */
    size_t selected_item_index;
    /*
        The items this widget can show. Populated from an Inventory struct
        using the ItemType member of this widget as the filter.
    */
    struct Item** items;
    /*
        The main panel border and background panel of this widget.
    */
    struct Panel* panel;
     /*
        The panel for the arrow icon that appears at the bottom of the widget
        when there are more items available to scroll down to.
    */
    struct Panel* bottom_scroll_arrow_panel;
    /*
        The panel for the arrow icon that appears at the top of the widget
        when there are more items available to scroll up to.
    */
    struct Panel* top_scroll_arrow_panel;
    /*
        The ItemSlotWidget of the currently selected item.
    */
    struct ItemSlotWidget* selected_item_slot_widget;
    /*
        The ItemSlotWidgets for all of the visible inventory items.
    */
    struct ItemSlotWidget* item_slot_widgets[INVENTORY_WIDGET_MAX_VISIBLE_ITEMS];
};

struct InventoryWidget* create_inventory_widget(struct GuiScreen* guiScreen);

void destroy_inventory_widget(struct InventoryWidget** widget);

void process_inventory_widget_input(
    struct InventoryWidget* widget,
    struct InputDevice* input_device);

void reset_inventory_widget(struct InventoryWidget* widget);

void set_inventory_widget_position(
    struct InventoryWidget* widget,
    const int x,
    const int y);

void update_inventory_widget(
    struct InventoryWidget* widget,
    const struct Inventory* inventory);

#endif