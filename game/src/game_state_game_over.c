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
        remove_actor_from_actor_list(game->World->Actors, game->World->Player.actor);
        destroy_actor(&game->World->Player.actor);
        game->World->Player.actor = NULL;
        CreatePlayerActor(game->World);

        BeginMapLoadTransition(game, "map01");
    }
}