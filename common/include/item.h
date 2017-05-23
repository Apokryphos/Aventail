#ifndef ITEM_HEADER_INCLUDED
#define ITEM_HEADER_INCLUDED

struct Item
{
    int TilesetId;
    char* Name;
};

struct Item* CreateItem(char* name);
void DestroyItem(struct Item** item);

#endif