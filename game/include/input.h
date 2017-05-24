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
    enum Direction MoveDirection;
    int Cancel;
    int CursorX;
    int CursorY;
    int Inventory;
    int Quit;
};

void UpdateInput(struct Input* input, struct InputDevice* inputDevice);

#endif