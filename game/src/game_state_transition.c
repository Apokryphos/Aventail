#include "direction.h"
#include "camera.h"
#include "game.h"
#include "render.h"
#include <assert.h>
#include <SDL2/SDL.h>

static enum GameState begin_game_state = GAME_STATE_NONE;
static enum GameState end_game_state = GAME_STATE_NONE;

static enum Direction transition_direction = DIRECTION_NONE;
static int transition_phase = 0;
static const float TRANSITION_DURATION = 0.66f;
static float transition_ticks = 0;

//  ---------------------------------------------------------------------------
void begin_game_state_transition(struct Game* game, enum GameState state)
{
    begin_game_state = game->state;
    end_game_state = state;

    game->state = GAME_STATE_TRANSITION;

    transition_direction = DIRECTION_NONE;
    transition_ticks = 0;
    transition_phase = 0;
}

//  ---------------------------------------------------------------------------
static void draw_transition_effect(struct Game* game)
{
    //Uint8 alpha = 255;
    float progress = (transition_ticks / TRANSITION_DURATION);

    draw_screen_fade(transition_phase == 0 ? progress : 1 - progress);

    // SDL_Rect dest_rect;
    // SDL_RenderGetViewport(game->renderer, &dest_rect);

    // switch (transition_direction)
    // {
    //     case DIRECTION_DOWN:
    //         dest_rect.y = transition_phase == 0 ?
    //         -dest_rect.h + (int)(dest_rect.h * progress) :
    //         (int)(dest_rect.h * progress);
    //         break;
    //     case DIRECTION_UP:
    //         dest_rect.y = transition_phase == 0 ?
    //         dest_rect.h - (int)(dest_rect.h * progress) :
    //         (int)(-dest_rect.h * progress);
    //         break;
    //     case DIRECTION_RIGHT:
    //         dest_rect.x = transition_phase == 0 ?
    //         -dest_rect.w + (int)(dest_rect.w * progress) :
    //         (int)(dest_rect.w * progress);
    //         break;
    //     case DIRECTION_LEFT:
    //         dest_rect.x = transition_phase == 0 ?
    //         dest_rect.w - (int)(dest_rect.w * progress) :
    //         (int)(-dest_rect.w * progress);
    //         break;
    //     case DIRECTION_NONE:
    //         alpha = transition_phase == 0 ?
    //         (int)(255 * progress) :
    //         255 - (int)(255 * progress);
    //         break;
    // }

    // if (alpha != 255)
    // {
    //     SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    // }

    // SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, alpha);
    // SDL_RenderFillRect(game->renderer, &dest_rect);

    // if (alpha != 255)
    // {
    //     SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_NONE);
    // }
}

//  ---------------------------------------------------------------------------
void draw_transition_game_state(struct Game* game)
{
    if (transition_phase == 0)
    {
        draw_game_state(game, begin_game_state, 1);
    }
    else
    {
        draw_game_state(game, end_game_state, 1);
    }

    draw_transition_effect(game);
}

//  ---------------------------------------------------------------------------
void update_transition_game_state(struct Game* game)
{
    assert(game != NULL);

    transition_ticks += game->elapsed_seconds;
    if (transition_ticks >= TRANSITION_DURATION)
    {
        transition_ticks -= TRANSITION_DURATION;
        ++transition_phase;

        if (transition_phase == 1)
        {
            //  Screen is blacked-out
            deactivate_game_state(game, begin_game_state);
            activate_game_state(game, end_game_state);
        }
        else
        {
            //  Transition is complete
            game->state = end_game_state;
        }
    }

    update_player_camera(game);
}