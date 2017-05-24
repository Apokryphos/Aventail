#ifndef GAME_STATE_LEVEL_HEADER_INCLUDED
#define GAME_STATE_LEVEL_HEADER_INCLUDED

struct Game;

void LevelGameStateDraw(struct Game* game, int inTransition);
void LevelGameStateUpdate(struct Game* game);

#endif