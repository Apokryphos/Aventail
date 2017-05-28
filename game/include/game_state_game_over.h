#ifndef GAME_STATE_GAME_OVER_HEADER_INCLUDED
#define GAME_STATE_GAME_OVER_HEADER_INCLUDED

struct Game;

void draw_game_over_state(struct Game* game, int in_transition);
void update_game_over_state(struct Game* game);

#endif