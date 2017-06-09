#ifndef ITEM_FUNCS_HEADER_INCLUDED
#define ITEM_FUNCS_HEADER_INCLUDED

struct Actor;
struct Item;

int use_heal_item(struct Item* item, struct Actor* target);

#endif