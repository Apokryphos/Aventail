#include "game.h"
#include "game_state.h"
#include "game_state_help.h"
#include "game_state_game_over.h"
#include "game_state_gear.h"
#include "game_state_level.h"
#include "game_state_load_map.h"
#include "game_state_main_menu.h"
#include "game_state_inventory.h"
#include "game_state_status.h"
#include "game_state_transition.h"
#include <assert.h>
#include <stdio.h>

//  ---------------------------------------------------------------------------
void activate_game_state(struct Game* game, const enum GameState game_state)
{
    switch (game_state)
    {
        case GAME_STATE_LEVEL:
            activate_level_game_state(game);
            break;

        case GAME_STATE_LOAD_MAP:
            activate_load_map_game_state(game);
            break;

        case GAME_STATE_GEAR:
            activate_gear_game_state(game);
            break;
        case GAME_STATE_INVENTORY:
            activate_inventory_game_state(game);
            break;
        case GAME_STATE_STATUS:
            activate_status_game_state(game);
            break;
        case GAME_STATE_MAIN_MENU:
            activate_main_menu_game_state(game);
            break;
        case GAME_STATE_HELP:
            activate_help_game_state(game);
            break;
        case GAME_STATE_GAME_OVER:
            activate_game_over_game_state(game);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_TRANSITION:
            //  Transition doesn't activate
            break;

        default:
            printf("activate_game_state: Unhandled GameState.\n");
            break;
    }
}

//  ---------------------------------------------------------------------------
void deactivate_game_state(struct Game* game, const enum GameState game_state)
{
    switch (game_state)
    {
        case GAME_STATE_LEVEL:
            deactivate_level_game_state(game);
            break;

        case GAME_STATE_LOAD_MAP:
            deactivate_load_map_game_state(game);
            break;

        case GAME_STATE_GEAR:
            deactivate_gear_game_state(game);
            break;
        case GAME_STATE_INVENTORY:
            deactivate_inventory_game_state(game);
            break;
        case GAME_STATE_STATUS:
            deactivate_status_game_state(game);
            break;
        case GAME_STATE_MAIN_MENU:
            deactivate_main_menu_game_state(game);
            break;
        case GAME_STATE_HELP:
            deactivate_help_game_state(game);
            break;
        case GAME_STATE_GAME_OVER:
            deactivate_game_over_game_state(game);
            break;

        case GAME_STATE_NONE:
        case GAME_STATE_TRANSITION:
            //  Transition doesn't deactivate
            break;

        default:
            printf("deactivate_game_state: Unhandled GameState.\n");
            break;
    }
}

//  ---------------------------------------------------------------------------
void draw_game_state(
    struct Game* game,
    const enum GameState game_state,
    const int in_transition)
{
    switch (game_state)
    {
        case GAME_STATE_LEVEL:
            draw_level_game_state(game, in_transition);
            break;

        case GAME_STATE_LOAD_MAP:
            draw_load_map_game_state(game, in_transition);
            break;

        case GAME_STATE_GEAR:
            draw_gear_game_state(game, in_transition);
            break;
        case GAME_STATE_INVENTORY:
            draw_inventory_game_state(game, in_transition);
            break;
        case GAME_STATE_STATUS:
            draw_status_game_state(game, in_transition);
            break;
        case GAME_STATE_MAIN_MENU:
            draw_main_menu_game_state(game, in_transition);
            break;
        case GAME_STATE_HELP:
            draw_help_game_state(game, in_transition);
            break;
        case GAME_STATE_GAME_OVER:
            draw_game_over_game_state(game, in_transition);
            break;
        case GAME_STATE_TRANSITION:
            assert(in_transition == 0);
            draw_transition_game_state(game);
            break;

        case GAME_STATE_NONE:
            break;

        default:
            printf("draw_active_game_state: Unhandled GameState.\n");
            break;
    }
}

//  ---------------------------------------------------------------------------
void enter_game_state(struct Game* game, const enum GameState dest_state)
{
    assert(game->state != dest_state);

    deactivate_game_state(game, game->state);

    activate_game_state(game, dest_state);

    game->state = dest_state;
}

//  ---------------------------------------------------------------------------
void update_active_game_state(struct Game* game)
{
    switch (game->state)
    {
        case GAME_STATE_LEVEL:
            update_level_game_state(game);
            break;

        case GAME_STATE_LOAD_MAP:
            update_load_map_game_state(game);
            break;

        case GAME_STATE_GEAR:
            update_gear_game_state(game);
            break;
        case GAME_STATE_INVENTORY:
            update_inventory_game_state(game);
            break;
        case GAME_STATE_STATUS:
            update_status_game_state(game);
            break;
        case GAME_STATE_MAIN_MENU:
            update_main_menu_game_state(game);
            break;
        case GAME_STATE_HELP:
            update_help_game_state(game);
            break;
        case GAME_STATE_GAME_OVER:
            update_game_over_game_state(game);
            break;
        case GAME_STATE_TRANSITION:
            update_transition_game_state(game);
            break;

        case GAME_STATE_NONE:
            break;

        default:
            printf("update_active_game_state: Unhandled GameState.\n");
            break;
    }
}