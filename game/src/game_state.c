#include "game.h"
#include "game_state.h"
#include "game_state_game_over.h"
#include "game_state_gear.h"
#include "game_state_level.h"
#include "game_state_inventory.h"
#include "game_state_status.h"
#include "game_state_transition.h"
#include <assert.h>
#include <stdio.h>

//  ---------------------------------------------------------------------------
void draw_active_game_state(
    struct Game* game,
    enum GameState game_state,
    int in_transition)
{
    assert(game_state != GAME_STATE_NONE);

    switch (game_state)
    {
        case GAME_STATE_LEVEL:
            LevelGameStateDraw(game, in_transition);
            break;
        case GAME_STATE_GEAR:
            GearGameStateDraw(game, in_transition);
            break;
        case GAME_STATE_INVENTORY:
            InventoryGameStateDraw(game, in_transition);
            break;
        case GAME_STATE_STATUS:
            StatusGameStateDraw(game, in_transition);
            break;
        case GAME_STATE_GAME_OVER:
            GameOverGameStateDraw(game, in_transition);
            break;
        case GAME_STATE_TRANSITION:
            assert(in_transition == 0);
            TransitionGameStateDraw(game);
            break;
        default:
            printf("draw_active_game_state: Unhandled GameState.\n");
            break;
    }
}

//  ---------------------------------------------------------------------------
void update_active_game_state(struct Game* game)
{
    assert(game->state != GAME_STATE_NONE);

    switch (game->state)
    {
        case GAME_STATE_LEVEL:
            LevelGameStateUpdate(game);
            break;
        case GAME_STATE_GEAR:
            GearGameStateUpdate(game);
            break;
        case GAME_STATE_INVENTORY:
            InventoryGameStateUpdate(game);
            break;
        case GAME_STATE_STATUS:
            StatusGameStateUpdate(game);
            break;
        case GAME_STATE_GAME_OVER:
            GameOverGameStateUpdate(game);
            break;
        case GAME_STATE_TRANSITION:
            TransitionGameStateUpdate(game);
            break;
        default:
            printf("update_active_game_state: Unhandled GameState.\n");
            break;
    }
}