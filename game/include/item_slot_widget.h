#ifndef ITEM_SLOT_WIDGET_HEADER_INCLUDED
#define ITEM_SLOT_WIDGET_HEADER_INCLUDED

#include "stat_type.h"

struct GuiScreen;
struct Item;
struct Panel;

struct ItemSlotWidget
{
    struct Panel* ItemIconPanel;
    struct Panel* ItemNamePanel;
    struct Panel* StatIconPanels[STAT_TYPE_COUNT];
    struct Panel* StatValuePanels[STAT_TYPE_COUNT];
    int StatValues[STAT_TYPE_COUNT];
    char* StatValueStrings[STAT_TYPE_COUNT];
};

struct ItemSlotWidget* CreateItemSlotWidget(struct GuiScreen* screen);

void DestroyItemSlotWidget(struct ItemSlotWidget** widget);

void SetItemSlotWidgetPosition(struct ItemSlotWidget* widget, int x, int y);

void SetItemSlotWidgetVisible(struct ItemSlotWidget* widget, int visible);

void UpdateItemSlotWidget(struct ItemSlotWidget* widget, struct Item* item);

#endif