#include "gear_slot_widget.h"
#include "item_slot_widget.h"

//  ---------------------------------------------------------------------------
void update_gear_slot_widget(
    struct GearSlotWidget* gear_slot_widget,
    struct Item* item)
{
    update_item_slot_widget(gear_slot_widget->item_slot_widget, item);
}