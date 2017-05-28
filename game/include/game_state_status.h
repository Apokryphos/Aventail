#ifndef GAME_STATE_STATUS_HEADER_INCLUDED
#define GAME_STATE_STATUS_HEADER_INCLUDED

struct Game;

void draw_status_game_state(struct Game* game, int in_transition);
void update_status_game_state(struct Game* game);

#endif