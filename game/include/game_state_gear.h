#ifndef GAME_STATE_GEAR_HEADER_INCLUDED
#define GAME_STATE_GEAR_HEADER_INCLUDED

struct Game;

void draw_gear_game_state(struct Game* game, int inTransition);
void update_gear_game_state(struct Game* game);

#endif