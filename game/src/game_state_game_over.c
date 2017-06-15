#include "actor.h"
#include "actor_list.h"
#include "game.h"
#include "render.h"
#include "simulate.h"
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

        respawn_player_actor(game);
    }
}