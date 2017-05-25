#ifndef ITEM_HEADER_INCLUDED
#define ITEM_HEADER_INCLUDED

#include "stats.h"

enum ItemType
{
    ITEM_TYPE_NONE,
    ITEM_TYPE_CONSUMABLE,
    ITEM_TYPE_ARMOR,
    ITEM_TYPE_WEAPON,
};

struct Item
{
    enum ItemType Type;
    int TilesetId;
    int HealAmount;
    char* Name;
    struct Stats Stats;
};

struct Item* CreateItem(const char* name);
void DestroyItem(struct Item** item);

#endif