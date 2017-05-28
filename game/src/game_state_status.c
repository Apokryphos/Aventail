#include "game.h"
#include "game_state.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
#include "panel.h"
#include "render.h"
#include "status_widget.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

struct GuiScreen* status_gui_screen = NULL;
struct StatusWidget* status_widget = NULL;

//  ---------------------------------------------------------------------------
static void exit_status_game_state(
    struct Game* game,
    enum GameState game_state)
{
    game->state = game_state;
    deactivate_gui();
    status_gui_screen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void process_status_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel || input_device->status)
    {
        exit_status_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->inventory)
    {
        exit_status_game_state(game, GAME_STATE_INVENTORY);
    }
    else if (input_device->gear)
    {
        exit_status_game_state(game, GAME_STATE_GEAR);
    }
}

//  ---------------------------------------------------------------------------
static void init_status_gui_screen(SDL_Renderer* renderer)
{
    status_gui_screen = CreateGuiScreen();

    status_widget = create_status_widget(status_gui_screen);

    //  Center status widget
    SDL_Rect viewport;
    SDL_RenderGetViewport(renderer, &viewport);
    set_status_widget_position(
        status_widget,
        (viewport.w / 2) - (status_widget->Panel->Width / 2),
        (viewport.h / 2) - (status_widget->Panel->Height / 2));

    add_gui_screen(status_gui_screen);
}

//  ---------------------------------------------------------------------------
void draw_status_game_state(struct Game* game, int in_transition)
{
    draw_map(game->renderer, game->world->map, game->world->actors);
    draw_gui(game);
}

//  ---------------------------------------------------------------------------
void update_status_game_state(struct Game* game)
{
    if (status_gui_screen == NULL)
    {
        init_status_gui_screen(game->renderer);
    }

    status_gui_screen->Enabled = 1;

    activate_gui();
    enable_gui_cursor(0);

    process_status_game_state_input(game);

    update_status_widget(status_widget, game->world->player.actor);
}