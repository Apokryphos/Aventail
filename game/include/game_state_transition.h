#ifndef GAME_STATE_TRANSITION_INCLUDED
#define GAME_STATE_TRANSITION_INCLUDED

#include "direction.h"

struct Game;
struct MapLink;

/*
    Begins a transition to another map.
    Direction specifies the direction of the transition wipe or DIRECTION_NONE
    for a fade.
*/
void BeginTransition(struct Game* game, struct MapLink* link, enum Direction direction);
void TransitionDraw(struct Game* game);
void TransitionUpdate(struct Game* game);

#endif