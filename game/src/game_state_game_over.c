#include "actor.h"
#include "actor_list.h"
#include "game.h"
#include "game_state_transition.h"
#include "world.h"
#include <stdlib.h>

static const float GameOverDuration = 1;

//  ---------------------------------------------------------------------------
void GameOverGameStateDraw(struct Game* game, int inTransition)
{
}

//  ---------------------------------------------------------------------------
void GameOverGameStateUpdate(struct Game* game)
{
    static float GameOverTicks = 0;

    GameOverTicks += game->ElapsedSeconds;
    if (GameOverTicks >= GameOverDuration)
    {
        GameOverTicks = 0;

        //  TODO: Player is destroyed and recreated here...hacky.
        RemoveActor(game->World->Actors, game->World->Player.Actor);
        DestroyActor(&game->World->Player.Actor);
        game->World->Player.Actor = NULL;
        CreatePlayerActor(game->World);

        BeginMapLoadTransition(game, "map01");
    }
}