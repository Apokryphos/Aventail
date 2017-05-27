#ifndef GAME_STATE_STATUS_HEADER_INCLUDED
#define GAME_STATE_STATUS_HEADER_INCLUDED

struct Game;

void StatusGameStateDraw(struct Game* game, int inTransition);
void StatusGameStateUpdate(struct Game* game);

#endif