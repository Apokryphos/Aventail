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
    GAME_STATE_STATUS,
    GAME_STATE_GEAR,
    GAME_STATE_GAME_OVER,
};

void draw_active_game_state(
    struct Game* game,
    enum GameState game_state,
    int in_transition);
void update_active_game_state(struct Game* game);

#endif