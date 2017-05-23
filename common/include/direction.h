#ifndef DIRECTION_HEADER_INCLUDED
#define DIRECTION_HEADER_INCLUDED

enum Direction
{
    DIRECTION_NONE,
    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_LEFT,
    DIRECTION_DOWN,
};

void GetDelta(const enum Direction direction, int* dx, int* dy);
enum Direction GetOppositeDirection(const enum Direction direction);
enum Direction GetRandomDirection();

#endif