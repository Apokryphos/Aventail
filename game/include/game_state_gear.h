#ifndef GAME_STATE_GEAR_HEADER_INCLUDED
#define GAME_STATE_GEAR_HEADER_INCLUDED

struct Game;

void draw_gear_game_state(struct Game* game, int in_transition);
void activate_gear_game_state(struct Game* game);
void deactivate_gear_game_state(struct Game* game);
void update_gear_game_state(struct Game* game);

#endif