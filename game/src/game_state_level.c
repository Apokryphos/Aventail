#include "game.h"
#include "actor.h"
#include "world.h"
#include "input.h"

//  ---------------------------------------------------------------------------
void ProcessLevelInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    game->World->Player.Actor->MoveDirection = inputDevice->MoveDirection;
    inputDevice->MoveDirection = DIRECTION_NONE;
}

//  ---------------------------------------------------------------------------
void LevelMain(struct Game* game)
{
    ProcessLevelInput(game);
    SimulateWorld(game, game->World);
}