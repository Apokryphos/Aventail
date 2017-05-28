#ifndef GAME_STATE_TRANSITION_INCLUDED
#define GAME_STATE_TRANSITION_INCLUDED

#include "direction.h"
#include "game_state.h"

struct Game;
struct MapLink;

/*
    Begins a transition to another GameState.
*/
void begin_game_state_transition(struct Game* game, enum GameState state);
/*
    Begins a transition to another map.
    Direction specifies the direction of the transition wipe or DIRECTION_NONE
    for a fade.
*/
void begin_map_link_transition(
    struct Game* game,
    struct MapLink* link,
    enum Direction direction);
void begin_map_load_transition(struct Game* game, const char* map_name);
void draw_transition_game_state(struct Game* game);
void update_transition_game_state(struct Game* game);

#endif