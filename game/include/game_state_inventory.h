#ifndef GAME_STATE_INVENTORY_HEADER_INCLUDED
#define GAME_STATE_INVENTORY_HEADER_INCLUDED

struct Game;

void InventoryGameStateDraw(struct Game* game, int inTransition);
void InventoryGameStateUpdate(struct Game* game);

#endif