//  asprintf
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "game.h"
#include "actor.h"
#include "actor_list.h"
#include "gui.h"
#include "inventory.h"
#include "input_device.h"
#include "item.h"
#include "map.h"
#include "player_hud.h"
#include "tile.h"
#include "render.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <strings.h>

//  Mouse cursor position. Used by is_mouse_cursor_over_actor.
static int mouse_cursor_x = 0;
static int mouse_cursor_y = 0;

static float hover_fade_ticks = 0;
static const float HOVER_FADE_DURATION = 0.33f;
static SDL_Rect hover_actor_rect;

static int hover_actor_health = 0;
static int hover_actor_max_health = 0;
static int hover_actor_cash = 0;
static enum ActorType hover_actor_type = ACTOR_TYPE_NONE;

static char* hover_actor_cash_string = NULL;
static char* hover_actor_health_string = NULL;
static char* hover_actor_name_string = NULL;

//  ---------------------------------------------------------------------------
void activate_level_game_state(struct Game* game)
{
    activate_player_hud(game);
}

//  ---------------------------------------------------------------------------
void deactivate_level_game_state(struct Game* game)
{
    deactivate_player_hud(game);
}

//  ---------------------------------------------------------------------------
static void process_level_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    game->world->player.actor->move_direction = input_device->move_direction;
    input_device->move_direction = DIRECTION_NONE;

    if (input_device->gear)
    {
        enter_game_state(game, GAME_STATE_GEAR);
    }
    else if (input_device->inventory)
    {
        enter_game_state(game, GAME_STATE_INVENTORY);
    }
    else if (input_device->status)
    {
        enter_game_state(game, GAME_STATE_STATUS);
    }
    else if (input_device->help)
    {
        enter_game_state(game, GAME_STATE_HELP);
    }

    if (input_device->debug_kill_player)
    {
        game->world->player.actor->health = 0;
    }
}

//  ---------------------------------------------------------------------------
int is_mouse_cursor_over_actor(struct Actor* actor)
{
    assert(actor != NULL);

    if ((actor->type == ACTOR_TYPE_PLAYER ||
        actor->type == ACTOR_TYPE_VILLAIN) &&
        actor->tile != NULL &&
        actor->health > 0)
    {
        SDL_Rect rect;
        get_tile_rect(actor->map, actor->tile, &rect, 1);

        return
            mouse_cursor_x >= rect.x &&
            mouse_cursor_x <= rect.x + rect.w &&
            mouse_cursor_y >= rect.y &&
            mouse_cursor_y <= rect.y +rect.h;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void draw_level_game_state(struct Game* game, int in_transition)
{
    if (game->world->map != NULL)
    {
        draw_map(game->world);
    }

    draw_gui(game);

    if (in_transition)
    {
        return;
    }

    mouse_cursor_x = game->input_device->mouse_cursor_x;
    mouse_cursor_y = game->input_device->mouse_cursor_y;

    struct Actor* hoverActor = find_actor_in_actor_list(
        game->world->actors,
        &is_mouse_cursor_over_actor);

    if (hoverActor != NULL)
    {
        hover_fade_ticks += game->elapsed_seconds;
        if (hover_fade_ticks > HOVER_FADE_DURATION)
        {
            hover_fade_ticks = HOVER_FADE_DURATION;
        }

        hover_actor_type = hoverActor->type;

        if (hoverActor->cash != hover_actor_cash)
        {
            hover_actor_cash = hoverActor->cash;

            if (hover_actor_cash_string != NULL)
            {
                free(hover_actor_cash_string);
                hover_actor_cash_string = NULL;
            }
        }

        if (hoverActor->health != hover_actor_health ||
            hoverActor->max_health != hover_actor_max_health)
        {
            hover_actor_health = hoverActor->health;
            hover_actor_max_health = hoverActor->max_health;

            if (hover_actor_health_string != NULL)
            {
                free(hover_actor_health_string);
                hover_actor_health_string = NULL;
            }
        }

        if (hover_actor_cash_string == NULL)
        {
            asprintf(&hover_actor_cash_string, "Cash: %d", hover_actor_cash);
        }

        if (hover_actor_health_string == NULL)
        {
            asprintf(&hover_actor_health_string, "Health: %d / %d", hover_actor_health, hover_actor_max_health);
        }

        if (hover_actor_name_string != NULL &&
            strcmp(hoverActor->name, hover_actor_name_string) != 0)
        {
            free(hover_actor_name_string);
            hover_actor_name_string = NULL;
        }

        if (hover_actor_name_string == NULL)
        {
            hover_actor_name_string = strdup(hoverActor->name);
        }

        get_tile_rect(hoverActor->map, hoverActor->tile, &hover_actor_rect, 0);
    }
    else
    {
        hover_fade_ticks -= game->elapsed_seconds;
        if (hover_fade_ticks < 0)
        {
            hover_fade_ticks = 0;
        }
    }

    if (hover_actor_health_string != NULL)
    {
        //float progress = hover_fade_ticks / HOVER_FADE_DURATION;

        //int alpha = (int)(255 * progress);

        int text_width, text_height;
        measure_text(hover_actor_health_string, &text_width, &text_height);

        const int line_height = text_height + 2;
        const int padding = 8;

        SDL_Rect dest_rect = hover_actor_rect;
        dest_rect.w = text_width + padding;
        dest_rect.h = line_height * 2;
        dest_rect.y -= hover_actor_rect.h + line_height;

        if (hover_actor_type == ACTOR_TYPE_PLAYER)
        {
            dest_rect.h += line_height;
            dest_rect.y -= line_height;
        }

        // if (alpha != 255)
        // {
        //     SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // }

        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        // SDL_RenderFillRect(renderer, &dest_rect);
        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
        // SDL_RenderDrawRect(renderer, &dest_rect);

        // draw_alpha_text(
        //     hover_actor_name_string,
        //     dest_rect.x + padding / 2,
        //     dest_rect.y + padding / 2,
        //     alpha);

        // draw_alpha_text(
        //     hover_actor_health_string,
        //     dest_rect.x + padding / 2,
        //     dest_rect.y + line_height + padding / 2,
        //     alpha);

        // if (hover_actor_type == ACTOR_TYPE_PLAYER)
        // {
        //     draw_alpha_text(
        //         hover_actor_cash_string,
        //         dest_rect.x + padding / 2,
        //         dest_rect.y + line_height + line_height + padding / 2,
        //         alpha);
        // }

        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // if (alpha != 255)
        // {
        //     SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        // }
    }
}

//  ---------------------------------------------------------------------------
void update_level_game_state(struct Game* game)
{
    process_level_game_state_input(game);
    simulate_world(game, game->world);
    update_player_hud(game);
}