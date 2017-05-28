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

    GameOverTicks += game->elapsed_seconds;
    if (GameOverTicks >= GameOverDuration)
    {
        GameOverTicks = 0;

        //  TODO: Player is destroyed and recreated here...hacky.
        remove_actor_from_actor_list(game->world->actors, game->world->player.actor);
        destroy_actor(&game->world->player.actor);
        game->world->player.actor = NULL;
        create_player_actor(game->world);

        BeginMapLoadTransition(game, "map01");
    }
}