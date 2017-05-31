#ifndef GAME_HEADER_INCLUDED
#define GAME_HEADER_INCLUDED

#include "game_state.h"

struct InputDevice;
struct World;

struct Game
{
    int quit;
    float elapsed_seconds;
    enum GameState state;
    struct InputDevice* input_device;
    struct World* world;
};

int game_main();

#endif