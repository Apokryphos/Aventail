#include "actor.h"
#include "actor_list.h"
#include "game.h"
#include "game_state_load_map.h"
#include "game_state_transition.h"
#include "render.h"
#include "world.h"
#include "zone.h"
#include <stdlib.h>

static const float GAME_OVER_DURATION = 1;

//  ---------------------------------------------------------------------------
void activate_game_over_game_state(struct Game* game)
{
}

//  ---------------------------------------------------------------------------
void deactivate_game_over_game_state(struct Game* game)
{
}

//  ---------------------------------------------------------------------------
void draw_game_over_game_state(struct Game* game, int in_transition)
{
    int text_x, text_y;
    get_viewport_center(&text_x, &text_y);
    draw_text_centered("GAME OVER", text_x, text_y);
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
        remove_actor_from_actor_list(game->world->zone->actors, game->world->player.actor);
        destroy_actor(&game->world->player.actor);
        game->world->player.actor = NULL;
        create_player_actor(game->world);

        queue_map_load("map01");
        begin_game_state_transition(game, GAME_STATE_LOAD_MAP);
    }
}