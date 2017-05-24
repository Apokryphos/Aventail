#ifndef GAME_STATE_HEADER_INCLUDED
#define GAME_STATE_HEADER_INCLUDED

struct Game;

enum GameState
{
    GAME_STATE_NONE,
    GAME_STATE_LEVEL,
    GAME_STATE_LOAD_MAP,
    GAME_STATE_TRANSITION,
    GAME_STATE_INVENTORY,
    GAME_STATE_GAME_OVER,
};

void DrawGameState(struct Game* game, enum GameState gameState, int inTransition);
void UpdateGameState(struct Game* game);

#endif