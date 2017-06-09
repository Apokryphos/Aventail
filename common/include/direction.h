#ifndef DIRECTION_HEADER_INCLUDED
#define DIRECTION_HEADER_INCLUDED

enum Direction
{
    DIRECTION_NONE,

    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_LEFT,
    DIRECTION_DOWN,

    DIRECTION_RIGHT_UP,
    DIRECTION_LEFT_UP,
    DIRECTION_RIGHT_DOWN,
    DIRECTION_LEFT_DOWN,
};

void get_direction_delta(const enum Direction direction, int* dx, int* dy);
const enum Direction get_direction_by_delta(
    int start_x,
    int start_y,
    int dest_x,
    int dest_y);
enum Direction get_opposite_direction(const enum Direction direction);
enum Direction get_random_direction();
int is_diagonal_direction(const enum Direction direction);

#endif