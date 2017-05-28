#ifndef ITEM_HEADER_INCLUDED
#define ITEM_HEADER_INCLUDED

#include "item_type.h"
#include "stats.h"

struct Item
{
    enum ItemType type;
    int tileset_id;
    int heal_amount;
    char* name;
    struct Stats stats;
};

struct Item* create_item(const char* name);
void destroy_item(struct Item** item);

#endif