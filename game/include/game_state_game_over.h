#ifndef GAME_STATE_GAME_OVER_HEADER_INCLUDED 
#define GAME_STATE_GAME_OVER_HEADER_INCLUDED 

struct Game;

void GameOverGameStateDraw(struct Game* game, int inTransition);
void GameOverGameStateUpdate(struct Game* game);

#endif