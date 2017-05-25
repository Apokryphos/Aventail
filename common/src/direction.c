#include "direction.h"
#include <stdlib.h>

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

//  ---------------------------------------------------------------------------
const enum Direction GetDirectionByDelta(
    int startX,
    int startY,
    int destX,
    int destY)
{
    int dx = destX - startX;
    int dy = destY - startY;

    if (dx == 0)
    {
        if (dy == 0)
        {
            return DIRECTION_NONE;
        }
        else if (dy < 0)
        {
            return DIRECTION_UP;
        }
        else 
        {
            return DIRECTION_DOWN;
        }
    }
    else if (dy == 0)
    {
        if (dx == 0)
        {
            return DIRECTION_NONE;
        }
        else if (dx < 0)
        {
            return DIRECTION_LEFT;
        }
        else 
        {
            return DIRECTION_RIGHT;
        }
    }

    //  Diagonals not supported
    return DIRECTION_NONE;
}

//  ---------------------------------------------------------------------------
enum Direction GetOppositeDirection(const enum Direction direction)
{
    switch (direction)
    {
        case DIRECTION_RIGHT:
            return DIRECTION_LEFT;

        case DIRECTION_UP:
            return DIRECTION_DOWN;

        case DIRECTION_LEFT:
            return DIRECTION_RIGHT;

        case DIRECTION_DOWN:
            return DIRECTION_UP;

        case DIRECTION_NONE:
        default:
            return DIRECTION_NONE;
    }
}

//  ---------------------------------------------------------------------------
enum Direction GetRandomDirection()
{
    //  Random direction excluding DIRECTION_NONE
    return (enum Direction)(rand() % 4 + 1);
}