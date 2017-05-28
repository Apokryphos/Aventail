#ifndef ITEM_DEFS_HEADER_INCLUDED
#define ITEM_DEFS_HEADER_INCLUDED

struct Item;

/*
    Debug function to create a quick weapon.
*/
struct Item* create_weapon(const char* name);
/*
    Loads item data for the specified item using it's name.
*/
void load_item_definition(struct Item* item);

#endif