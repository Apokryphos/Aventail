#ifndef GAME_STATE_INVENTORY_HEADER_INCLUDED
#define GAME_STATE_INVENTORY_HEADER_INCLUDED

struct Game;

void draw_inventory_game_state(struct Game* game, int in_transition);
void update_inventory_game_state(struct Game* game);

#endif