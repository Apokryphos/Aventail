#ifndef INVENTORY_HEADER_INCLUDED
#define INVENTORY_HEADER_INCLUDED

#include "item.h"
#include <stddef.h>

extern const size_t MAX_INVENTORY_ITEMS;

struct Inventory
{
    struct Item** items;
};

int add_item_to_inventory(struct Inventory* inventory, struct Item* item);
struct Inventory* create_inventory();
void destroy_inventory(struct Inventory** inventory);
size_t get_inventory_item_count(const struct Inventory* inventory);
void  get_inventory_items_by_item_type(
    const struct Inventory* inventory,
    const enum ItemType item_type,
    struct Item** items,
    size_t* count);
int is_inventory_full();
int move_inventory_items(struct Inventory* source, struct Inventory* dest);
int remove_item_from_inventory(struct Inventory* inventory, struct Item* item);

#endif