#ifndef GAME_STATE_LEVEL_HEADER_INCLUDED
#define GAME_STATE_LEVEL_HEADER_INCLUDED

struct Game;

void draw_level_game_state(struct Game* game, int in_transition);
void activate_level_game_state(struct Game* game);
void deactivate_level_game_state(struct Game* game);
void update_level_game_state(struct Game* game);

#endif