#ifndef INVENTORY_HEADER_INCLUDED
#define INVENTORY_HEADER_INCLUDED

#include "item.h"
#include <stddef.h>

extern const size_t MaxInventoryItems;

struct Inventory
{
    struct Item** Items;
};

int AddInventoryItem(struct Inventory* inventory, struct Item* item);
struct Inventory* CreateInventory();
void DestroyInventory(struct Inventory** inventory);
size_t GetInventoryItemCount(const struct Inventory* inventory);
int GiveInventoryItems(struct Inventory* source, struct Inventory* dest);
int InventoryIsFull();
int RemoveInventoryItem(struct Inventory* inventory, struct Item* item);

#endif