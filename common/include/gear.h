#ifndef GEAR_HEADER_INCLUDED
#define GEAR_HEADER_INCLUDED

#include "item_type.h"
#include "stats.h"

struct Actor;
struct Item;

struct Gear
{
    struct Item* armor;
    struct Item* shield;
    struct Item* weapon;
    struct Item* accessory;
    struct Stats stats;
};

int equip_item(struct Actor* actor, struct Item* item);
int remove_item_from_gear(struct Actor* actor, enum ItemType item_type);
void remove_all_items_from_gear(struct Actor* actor);

#endif