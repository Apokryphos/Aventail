#include "actor.h"
#include "gear.h"
#include "inventory.h"
#include "item.h"
#include <assert.h>

//  ---------------------------------------------------------------------------
static struct Item** GetItemSlot(struct Gear* gear, enum ItemType item_type)
{
    switch (item_type)
    {
        case ITEM_TYPE_WEAPON:
            return &gear->weapon;

        case ITEM_TYPE_ARMOR:
            return &gear->armor;

        case ITEM_TYPE_SHIELD:
            return &gear->shield;

        case ITEM_TYPE_ACCESSORY:
            return &gear->accessory;

        default:
            return NULL;
    }
}

//  ---------------------------------------------------------------------------
static struct Stats GetItemSlotStats(struct Item* item)
{
    if (item != NULL)
    {
        return item->Stats;
    }
    return (struct Stats) { 0 };
}

//  ---------------------------------------------------------------------------
static void CalculateGearStats(struct Gear* gear)
{
    struct Stats weapon_stats = GetItemSlotStats(gear->weapon);
    struct Stats armor_stats = GetItemSlotStats(gear->armor);
    struct Stats shield_stats = GetItemSlotStats(gear->shield);
    struct Stats accessory_stats = GetItemSlotStats(gear->accessory);

    gear->stats = (struct Stats) { 0 };
    gear->stats = AddStats(gear->stats, weapon_stats);
    gear->stats = AddStats(gear->stats, armor_stats);
    gear->stats = AddStats(gear->stats, shield_stats);
    gear->stats = AddStats(gear->stats, accessory_stats);
}

//  ---------------------------------------------------------------------------
static int RemoveItemFromSlot(
    struct Item** item_slot,
    struct Inventory* inventory)
{
    if (*item_slot != NULL)
    {
        AddInventoryItem(inventory, *item_slot);
        *item_slot = NULL;
        return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int equip_item(struct Actor* actor, struct Item* item)
{
    assert(item != NULL);
    assert(actor != NULL);

    struct Gear* gear = &actor->gear;
    struct Inventory* inventory = actor->inventory;

    struct Item** item_slot = GetItemSlot(gear, item->Type);

    if (item_slot != NULL)
    {
        RemoveItemFromSlot(item_slot, inventory);
        *item_slot = item;
        CalculateGearStats(gear);
        return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int remove_item_from_gear(struct Actor* actor, enum ItemType item_type)
{
    assert(actor != NULL);

    struct Gear* gear = &actor->gear;
    struct Inventory* inventory = actor->inventory;

    struct Item** item_slot = GetItemSlot(gear, item_type);
    if (item_slot != NULL)
    {
        int removed = RemoveItemFromSlot(item_slot, inventory);
        CalculateGearStats(gear);
        return removed;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void remove_all_items_from_gear(struct Actor* actor)
{
    remove_item_from_gear(actor, ITEM_TYPE_WEAPON);
    remove_item_from_gear(actor, ITEM_TYPE_ARMOR);
    remove_item_from_gear(actor, ITEM_TYPE_SHIELD);
    remove_item_from_gear(actor, ITEM_TYPE_ACCESSORY);
}
