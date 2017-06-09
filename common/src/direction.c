#include "direction.h"
#include <stdlib.h>

//  ---------------------------------------------------------------------------
void get_direction_delta(const enum Direction direction, int* dx, int* dy)
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

        case DIRECTION_LEFT_DOWN:
            *dx = -1;
            *dy = 1;
            return;

        case DIRECTION_LEFT_UP:
            *dx = -1;
            *dy = -1;
            return;

        case DIRECTION_RIGHT_DOWN:
            *dx = 1;
            *dy = 1;
            return;

        case DIRECTION_RIGHT_UP:
            *dx = 1;
            *dy = -1;
            return;

        case DIRECTION_NONE:
        default:
            *dx = 0;
            *dy = 0;
            return;
    }
}

//  ---------------------------------------------------------------------------
const enum Direction get_direction_by_delta(
    int start_x,
    int start_y,
    int dest_x,
    int dest_y)
{
    int dx = dest_x - start_x;
    int dy = dest_y - start_y;

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
enum Direction get_opposite_direction(const enum Direction direction)
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
enum Direction get_random_direction()
{
    //  Random direction excluding DIRECTION_NONE
    return (enum Direction)(rand() % 4 + 1);
}

//  ---------------------------------------------------------------------------
int is_diagonal_direction(const enum Direction direction)
{
    switch (direction)
    {
        default:
        case DIRECTION_NONE:
        case DIRECTION_RIGHT:
        case DIRECTION_UP:
        case DIRECTION_LEFT:
        case DIRECTION_DOWN:
            return 0;

        case DIRECTION_LEFT_DOWN:
        case DIRECTION_LEFT_UP:
        case DIRECTION_RIGHT_DOWN:
        case DIRECTION_RIGHT_UP:
            return 1;
    }
}