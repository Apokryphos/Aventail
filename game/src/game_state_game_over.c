#include "actor.h"
#include "actor_list.h"
#include "game.h"
#include "game_state_transition.h"
#include "world.h"
#include <stdlib.h>

static const float GAME_OVER_DURATION = 1;

//  ---------------------------------------------------------------------------
void draw_game_over_game_state(struct Game* game, int in_transition)
{
}

//  ---------------------------------------------------------------------------
void update_game_over_game_state(struct Game* game)
{
    static float ticks = 0;

    ticks += game->elapsed_seconds;
    if (ticks >= GAME_OVER_DURATION)
    {
        ticks = 0;

        //  TODO: Player is destroyed and recreated here...hacky.
        remove_actor_from_actor_list(game->world->actors, game->world->player.actor);
        destroy_actor(&game->world->player.actor);
        game->world->player.actor = NULL;
        create_player_actor(game->world);

        begin_map_load_transition(game, "map01");
    }
}