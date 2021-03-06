#ifndef GAME_STATE_STATUS_HEADER_INCLUDED
#define GAME_STATE_STATUS_HEADER_INCLUDED

struct Game;

void draw_status_game_state(struct Game* game, int in_transition);
void activate_status_game_state(struct Game* game);
void deactivate_status_game_state(struct Game* game);
void update_status_game_state(struct Game* game);

#endif