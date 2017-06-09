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
static void process_status_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel || input_device->status)
    {
        enter_game_state(game, GAME_STATE_LEVEL);
    }
    else if (input_device->inventory)
    {
        enter_game_state(game, GAME_STATE_INVENTORY);
    }
    else if (input_device->gear)
    {
        enter_game_state(game, GAME_STATE_GEAR);
    }
    else if (input_device->help)
    {
        enter_game_state(game, GAME_STATE_HELP);
    }
}

//  ---------------------------------------------------------------------------
static void init_status_gui_screen()
{
    status_gui_screen = create_gui_screen();

    status_widget = create_status_widget(status_gui_screen);

    //  Center status widget
    SDL_Rect viewport;
    get_viewport(&viewport);
    set_status_widget_position(
        status_widget,
        (viewport.w / 2) - (status_widget->panel->width / 2),
        (viewport.h / 2) - (status_widget->panel->height / 2));

    add_gui_screen(status_gui_screen);
}

//  ---------------------------------------------------------------------------
void activate_status_game_state(struct Game* game)
{
    if (status_gui_screen == NULL)
    {
        init_status_gui_screen();
    }

    activate_gui();
    enable_gui_cursor(0);
    status_gui_screen->enabled = 1;
}

//  ---------------------------------------------------------------------------
void deactivate_status_game_state(struct Game* game)
{
    deactivate_gui();
    status_gui_screen->enabled = 0;
}

//  ---------------------------------------------------------------------------
void draw_status_game_state(struct Game* game, int in_transition)
{
    draw_map(game->world);
    draw_gui(game);
}

//  ---------------------------------------------------------------------------
void update_status_game_state(struct Game* game)
{
    process_status_game_state_input(game);
    update_status_widget(status_widget, game->world->player.actor);
}