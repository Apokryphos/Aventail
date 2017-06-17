#ifndef INPUT_DEVICE_HEADER_INCLUDED
#define INPUT_DEVICE_HEADER_INCLUDED

#include "direction.h"

struct InputDevice
{
    enum Direction move_direction;
    int accept;
    int cancel;
    int mouse_cursor_x;
    int mouse_cursor_y;
    int help;
    int inventory;
    int gear;
    int remove;
    int status;
    int quit;
    int wait;
    int debug_kill_player;
};

#endif