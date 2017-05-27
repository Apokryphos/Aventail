#ifndef ITEM_HEADER_INCLUDED
#define ITEM_HEADER_INCLUDED

#include "item_type.h"
#include "stats.h"

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