#ifndef INPUT_HEADER_INCLUDED
#define INPUT_HEADER_INCLUDED

#include "direction.h"
#include <SDL2/SDL_events.h>

struct Input
{
    SDL_Event Event;
};

struct InputDevice
{
    int CursorX;
    int CursorY;
    enum Direction MoveDirection;
    int Quit;
};

void UpdateInput(struct Input* input, struct InputDevice* inputDevice);

#endif