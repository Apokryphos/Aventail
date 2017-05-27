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
    enum ItemType ItemType;
    size_t ItemCount;
    size_t SelectedSlotIndex;
    struct ItemSlotWidget* SelectedItemSlotWidget;
    struct Item** Items;
    struct Panel* Panel;
    struct Panel* ScrollArrowBottomPanel;
    struct Panel* ScrollArrowTopPanel;
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