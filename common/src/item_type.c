#include "item_type.h"

//  ---------------------------------------------------------------------------
const char* GetItemTypeCategoryString(const enum ItemType itemType)
{
    switch (itemType)
    {
        default:
            return "Unknown ItemType";
        case ITEM_TYPE_NONE:
            return "All Items";
        case ITEM_TYPE_CONSUMABLE:
            return "Consumables";
        case ITEM_TYPE_ARMOR:
            return "Armor";
        case ITEM_TYPE_SHIELD:
            return "Shields";
        case ITEM_TYPE_WEAPON:
            return "Weapons";
        case ITEM_TYPE_ACCESSORY:
            return "Accessories";
    }
}