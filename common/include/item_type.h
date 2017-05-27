#ifndef ITEM_TYPE_HEADER_INCLUDED
#define ITEM_TYPE_HEADER_INCLUDED

#define ITEM_TYPE_COUNT 6

enum ItemType
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_CONSUMABLE,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_SHIELD,
    ITEM_TYPE_WEAPON,
    ITEM_TYPE_ACCESSORY,
};

const char* GetItemTypeCategoryString(const enum ItemType itemType);

#endif