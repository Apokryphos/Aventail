#ifndef GAME_STATE_HELP_INCLUDED
#define GAME_STATE_HELP_INCLUDED

#include "game_state.h"

struct Game;

void draw_help_game_state(struct Game* game, int in_transition);
void activate_help_game_state(struct Game* game);
void deactivate_help_game_state(struct Game* game);
void update_help_game_state(struct Game* game);

#endif