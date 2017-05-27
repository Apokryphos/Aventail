#include "actor.h"
#include "gear.h"
#include "inventory.h"
#include "item.h"
#include <assert.h>

//  ---------------------------------------------------------------------------
static struct Item** GetItemSlot(struct Gear* gear, enum ItemType itemType)
{
    switch (itemType)
    {
        case ITEM_TYPE_WEAPON:
            return &gear->Weapon;

        case ITEM_TYPE_ARMOR:
            return &gear->Armor;

        case ITEM_TYPE_SHIELD:
            return &gear->Shield;

        case ITEM_TYPE_ACCESSORY:
            return &gear->Accessory;

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
    struct Stats weaponStats = GetItemSlotStats(gear->Weapon);
    struct Stats armorStats = GetItemSlotStats(gear->Armor);
    struct Stats shieldStats = GetItemSlotStats(gear->Shield);
    struct Stats accessoryStats = GetItemSlotStats(gear->Accessory);

    gear->Stats = (struct Stats) { 0 };
    gear->Stats = AddStats(gear->Stats, weaponStats);
    gear->Stats = AddStats(gear->Stats, armorStats);
    gear->Stats = AddStats(gear->Stats, shieldStats);
    gear->Stats = AddStats(gear->Stats, accessoryStats);
}

//  ---------------------------------------------------------------------------
static int RemoveItemFromSlot(struct Item** itemSlot, struct Inventory* inventory)
{
    if (*itemSlot != NULL)
    {
        AddInventoryItem(inventory, *itemSlot);
        *itemSlot = NULL;
        return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int EquipItem(struct Actor* actor, struct Item* item)
{
    assert(item != NULL);
    assert(actor != NULL);

    struct Gear* gear = &actor->Gear;
    struct Inventory* inventory = actor->Inventory;

    struct Item** itemSlot = GetItemSlot(gear, item->Type);

    if (itemSlot != NULL)
    {
        RemoveItemFromSlot(itemSlot, inventory);
        *itemSlot = item;
        CalculateGearStats(gear);
        return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
int RemoveGearItem(struct Actor* actor, enum ItemType itemType)
{
    assert(actor != NULL);

    struct Gear* gear = &actor->Gear;
    struct Inventory* inventory = actor->Inventory;

    struct Item** itemSlot = GetItemSlot(gear, itemType);
    if (itemSlot != NULL)
    {
        int removed = RemoveItemFromSlot(itemSlot, inventory);
        CalculateGearStats(gear);
        return removed;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void RemoveAllGearItems(struct Actor* actor)
{
    RemoveGearItem(actor, ITEM_TYPE_WEAPON);
    RemoveGearItem(actor, ITEM_TYPE_ARMOR);
    RemoveGearItem(actor, ITEM_TYPE_SHIELD);
    RemoveGearItem(actor, ITEM_TYPE_ACCESSORY);
}
