#include "input.h"
#include <SDL2/SDL.h>

//  ---------------------------------------------------------------------------
void UpdateInput(struct InputDevice* inputDevice)
{
    //  Reset
    inputDevice->MoveDirection = DIRECTION_NONE;
    inputDevice->Cancel = 0;
    inputDevice->Inventory = 0;
    inputDevice->DebugKillPlayerActor = 0;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                inputDevice->Quit = 1;
                break;

            case SDL_KEYDOWN:
                {
                    SDL_Keycode key = event.key.keysym.sym;
                    switch (key)
                    {
                        case SDLK_q:
                            inputDevice->Quit = 1;
                            break;
                        case SDLK_ESCAPE:
                            inputDevice->Cancel = 1;
                            break;
                        case SDLK_i:
                            inputDevice->Inventory = 1;
                            break;
                        case SDLK_k:
                            inputDevice->DebugKillPlayerActor = 1;
                            break;
                        case SDLK_UP:
                            inputDevice->MoveDirection = DIRECTION_UP;
                            break;
                        case SDLK_DOWN:
                            inputDevice->MoveDirection = DIRECTION_DOWN;
                            break;
                        case SDLK_LEFT:
                            inputDevice->MoveDirection = DIRECTION_LEFT;
                            break;
                        case SDLK_RIGHT:
                            inputDevice->MoveDirection = DIRECTION_RIGHT;
                            break;
                    }
                }
                break;

                case SDL_MOUSEMOTION:
                    inputDevice->CursorX = event.motion.x;
                    inputDevice->CursorY = event.motion.y;
                    break;

            default:
            break;
        }
    }
}