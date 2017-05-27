#ifndef INPUT_HEADER_INCLUDED
#define INPUT_HEADER_INCLUDED

#include "direction.h"
#include <SDL2/SDL_events.h>

struct InputDevice
{
    enum Direction MoveDirection;
    int Accept;
    int Cancel;
    int CursorX;
    int CursorY;
    int Inventory;
    int Gear;
    int Remove;
    int Quit;
    int DebugKillPlayerActor;
};

void UpdateInput(struct InputDevice* inputDevice);

#endif