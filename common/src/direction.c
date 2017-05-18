#include "direction.h"

//  ---------------------------------------------------------------------------
void GetDelta(const enum Direction direction, int* dx, int* dy)
{
    switch (direction)
    {
        case DIRECTION_RIGHT:
            *dx = 1;
            *dy = 0;
            return;

        case DIRECTION_UP:
            *dx = 0;
            *dy = -1;
            return;

        case DIRECTION_LEFT:
            *dx = -1;
            *dy = 0;
            return;

        case DIRECTION_DOWN:
            *dx = 0;
            *dy = 1;
            return;

        case DIRECTION_NONE:
        default:
            *dx = 0;
            *dy = 0;
            return;
    }
}