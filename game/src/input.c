#include "input_device.h"
#include <SDL2/SDL.h>

//  ---------------------------------------------------------------------------
void update_input(struct InputDevice* input_device)
{
    //  Reset
    input_device->move_direction = DIRECTION_NONE;
    input_device->cancel = 0;
    input_device->gear = 0;
    input_device->inventory = 0;
    input_device->debug_kill_player = 0;
    input_device->accept = 0;
    input_device->remove = 0;
    input_device->status = 0;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                input_device->quit = 1;
                break;

            case SDL_KEYDOWN:
                {
                    SDL_Keycode key = event.key.keysym.sym;
                    switch (key)
                    {
                        case SDLK_q:
                            input_device->quit = 1;
                            break;
                        case SDLK_ESCAPE:
                            input_device->cancel = 1;
                            break;
                        case SDLK_RETURN:
                            input_device->accept = 1;
                            break;
                        case SDLK_r:
                            input_device->remove = 1;
                            break;
                        case SDLK_g:
                            input_device->gear = 1;
                            break;
                        case SDLK_i:
                            input_device->inventory = 1;
                            break;
                        case SDLK_k:
                            input_device->debug_kill_player = 1;
                            break;
                        case SDLK_s:
                            input_device->status = 1;
                            break;
                        case SDLK_UP:
                            input_device->move_direction = DIRECTION_UP;
                            break;
                        case SDLK_DOWN:
                            input_device->move_direction = DIRECTION_DOWN;
                            break;
                        case SDLK_LEFT:
                            input_device->move_direction = DIRECTION_LEFT;
                            break;
                        case SDLK_RIGHT:
                            input_device->move_direction = DIRECTION_RIGHT;
                            break;
                    }
                }
                break;

                case SDL_MOUSEMOTION:
                    input_device->mouse_cursor_x = event.motion.x;
                    input_device->mouse_cursor_y = event.motion.y;
                    break;

            default:
            break;
        }
    }
}