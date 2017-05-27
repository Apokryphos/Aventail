#ifndef INVENTORY_WIDGET_HEADER_INCLUDED
#define INVENTORY_WIDGET_HEADER_INCLUDED

#include "blink_state_data.h"
#include "item_type.h"
#include <stddef.h>

struct GuiScreen;
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
    enum ItemType ItemType;
    /*
        The number of items in the Items member.
    */
    size_t ItemCount;
    /*
        The index into the Items member of the currently selected item.
    */
    size_t SelectedItemIndex;
    /*
        The items this widget can show. Populated from an Inventory struct
        using the ItemType member of this widget as the filter.
    */
    struct Item** Items;
    /*
        The main panel border and background panel of this widget.
    */
    struct Panel* Panel;
     /*
        The panel for the arrow icon that appears at the bottom of the widget
        when there are more items available to scroll down to.
    */
    struct Panel* ScrollArrowBottomPanel;
    /*
        The panel for the arrow icon that appears at the top of the widget
        when there are more items available to scroll up to.
    */
    struct Panel* ScrollArrowTopPanel;
    /*
        The ItemSlotWidget of the currently selected item.
    */
    struct ItemSlotWidget* SelectedItemSlotWidget;
    /*
        The ItemSlotWidgets for all of the visible inventory items.
    */
    struct ItemSlotWidget* ItemSlotWidgets[INVENTORY_WIDGET_MAX_VISIBLE_ITEMS];
};

struct InventoryWidget* CreateInventoryWidget(struct GuiScreen* guiScreen);

void DestroyInventoryWidget(struct InventoryWidget** widget);

void SelectNextInventoryWidgetItemSlot(struct InventoryWidget* widget);

void SelectPreviousInventoryWidgetItemSlot(struct InventoryWidget* widget);

void SetInventoryWidgetPosition(struct InventoryWidget* widget, int x, int y);

void UpdateInventoryWidget(
    struct InventoryWidget* widget,
    struct Inventory* inventory);

#endif