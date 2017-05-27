#ifndef ITEM_DEFS_HEADER_INCLUDED
#define ITEM_DEFS_HEADER_INCLUDED

struct Item;

/*
    Debug function to create a quick weapon.
*/
struct Item* CreateWeapon(const char* name);
/*
    Loads item data for the specified item using it's name.
*/
void LoadItemDefinition(struct Item* item);

#endif