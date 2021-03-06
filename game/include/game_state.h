#ifndef GAME_STATE_HEADER_INCLUDED
#define GAME_STATE_HEADER_INCLUDED

struct Game;

enum GameState
{
    GAME_STATE_NONE,

    GAME_STATE_LEVEL,
    GAME_STATE_TRANSITION,
    GAME_STATE_LOAD_MAP,
    GAME_STATE_GAME_OVER,

    //  GUI states
    GAME_STATE_MAIN_MENU,
    GAME_STATE_HELP,
    GAME_STATE_GEAR,
    GAME_STATE_INVENTORY,
    GAME_STATE_STATUS,
};

/*
*   Activates the specified game state.
*   Do not call directly.
*/
void activate_game_state(struct Game* game, const enum GameState game_state);

/*
*   Deactivates the specified game state.
*   Do not call directly.
*/
void deactivate_game_state(struct Game* game, const enum GameState game_state);

/*
*   Enters the specified game state immediately.
*   Mostly used for entering GUI states.
*/
void enter_game_state(struct Game* game, const enum GameState dest_state);

/*
*   Draws the specified game state.
*
*   During transitions, the specified game state might not be the same as the
*   game's state member.
*/
void draw_game_state(
    struct Game* game,
    const enum GameState game_state,
    const int in_transition);

/*
*   Updates the state specified by the game argument's state member.
*/
void update_active_game_state(struct Game* game);

#endif