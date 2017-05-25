#ifndef ITEM_HEADER_INCLUDED
#define ITEM_HEADER_INCLUDED

#include "stats.h"

struct Item
{
    int TilesetId;
    char* Name;
    int HealAmount;
    struct Stats Stats;
};

struct Item* CreateItem(const char* name);
void DestroyItem(struct Item** item);

#endif