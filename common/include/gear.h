#ifndef GEAR_HEADER_INCLUDED
#define GEAR_HEADER_INCLUDED

#include "item_type.h"
#include "stats.h"

struct Actor;
struct Item;

struct Gear
{
    struct Item* Armor;
    struct Item* Shield;
    struct Item* Weapon;
    struct Item* Accessory;
    struct Stats Stats;
};

int EquipItem(struct Actor* actor, struct Item* item);
int RemoveGearItem(struct Actor* actor, enum ItemType itemType);
void RemoveAllGearItems(struct Actor* actor);

#endif