#ifndef ITEM_HEADER_INCLUDED
#define ITEM_HEADER_INCLUDED

#include "item_type.h"
#include "stats.h"

struct Actor;
struct Item;

/*
*   Returns zero on successful usage or non-zero if the item wasn't used.
*   Inventory game state will play an error sound effect if non-zero is
*   returned.
*/
typedef int ItemOnUseFunction(struct Item* item, struct Actor* target);

struct Item
{
    enum ItemType type;
    int tileset_id;
    int heal_amount;
    char* name;
    ItemOnUseFunction* on_use;
    struct Stats stats;
};

struct Item* create_item(const char* name);
void destroy_item(struct Item** item);

#endif