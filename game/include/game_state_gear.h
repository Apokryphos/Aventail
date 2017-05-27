#ifndef GAME_STATE_GEAR_HEADER_INCLUDED
#define GAME_STATE_GEAR_HEADER_INCLUDED

struct Game;

void GearGameStateDraw(struct Game* game, int inTransition);
void GearGameStateUpdate(struct Game* game);

#endif