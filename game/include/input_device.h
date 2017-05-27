#ifndef INPUT_DEVICE_HEADER_INCLUDED
#define INPUT_DEVICE_HEADER_INCLUDED

#include "direction.h"

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
    int Status;
    int Quit;
    int DebugKillPlayerActor;
};

#endif