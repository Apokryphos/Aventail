#include "game.h"
#include "game_state.h"
#include "game_state_game_over.h"
#include "game_state_level.h"
#include "game_state_inventory.h"
#include "game_state_transition.h"
#include <assert.h>
#include <stdio.h>

//  ---------------------------------------------------------------------------
void DrawGameState(struct Game* game, enum GameState gameState, int inTransition)
{
    assert(gameState != GAME_STATE_NONE);

    switch (gameState)
    {
        case GAME_STATE_LEVEL:
            LevelGameStateDraw(game, inTransition);
            break;
        case GAME_STATE_INVENTORY:
            InventoryGameStateDraw(game, inTransition);
            break; 
        case GAME_STATE_GAME_OVER:
            GameOverGameStateDraw(game, inTransition);
            break;
        case GAME_STATE_TRANSITION:
            assert(inTransition == 0);
            TransitionGameStateDraw(game);
            break;
        default:
            printf("DrawGameState: Unhandled GameState.\n");
            break;
    }
}

//  ---------------------------------------------------------------------------
void UpdateGameState(struct Game* game)
{
    assert(game->State != GAME_STATE_NONE);

    switch (game->State)
    {
        case GAME_STATE_LEVEL:
            LevelGameStateUpdate(game);
            break;
        case GAME_STATE_INVENTORY:
            InventoryGameStateUpdate(game);
            break;
        case GAME_STATE_GAME_OVER:
            GameOverGameStateUpdate(game);
            break;
        case GAME_STATE_TRANSITION:
            TransitionGameStateUpdate(game);
            break;
        default:
            printf("UpdateGameState: Unhandled GameState.\n");
            break;
    }
}