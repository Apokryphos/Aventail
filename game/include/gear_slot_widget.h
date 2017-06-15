#ifndef GEAR_SLOT_WIDGET_HEADER
#define GEAR_SLOT_WIDGET_HEADER

struct ItemSlotWidget;
struct Panel;

struct GearSlotWidget
{
    int index;
    struct ItemSlotWidget* item_slot_widget;
    struct Panel* slot_panel;
};

#endif