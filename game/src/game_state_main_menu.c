#include "game.h"
#include "game_state.h"
#include "game_state_help.h"
#include "game_state_load_map.h"
#include "game_state_transition.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
#include "panel.h"
#include "render.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

struct GuiScreen* menu_gui_screen = NULL;

//  ---------------------------------------------------------------------------
static void process_menu_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->accept)
    {
        queue_map_load("surface01");
        begin_game_state_transition(game, GAME_STATE_LOAD_MAP);
    }
    else if (input_device->help)
    {
        enter_game_state(game, GAME_STATE_HELP);
    }
    else if (input_device->cancel)
    {
        game->quit = 1;
    }
}

//  ---------------------------------------------------------------------------
static void init_menu_gui_screen()
{
    menu_gui_screen = create_gui_screen();

    const char* title_text = "Aventail";
    int title_width, title_height;
    measure_text(title_text, &title_width, &title_height);

    struct Panel* title_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    title_panel->text = title_text;
    title_panel->width = title_width;
    title_panel->height = title_height;
    add_panel_to_gui_screen(menu_gui_screen, title_panel);

    const char* start_text = "Press Enter to start.";
    int start_width, start_height;
    measure_text(start_text, &start_width, &start_height);

    struct Panel* start_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    start_panel->text = start_text;
    start_panel->width = start_width;
    start_panel->height = start_height;
    add_panel_to_gui_screen(menu_gui_screen, start_panel);

    const char* help_text = "Press ? at any time for help.";
    int help_width, help_height;
    measure_text(help_text, &help_width, &help_height);

    struct Panel* help_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    help_panel->text = help_text;
    help_panel->width = help_width;
    help_panel->height = help_height;
    add_panel_to_gui_screen(menu_gui_screen, help_panel);

    //  Center title
    SDL_Rect viewport;
    get_viewport(&viewport);
    title_panel->x = (viewport.w / 2) - (title_panel->width / 2);
    title_panel->y = (viewport.h / 2) - (title_panel->height / 2);

    start_panel->x = (viewport.w / 2) - (start_panel->width / 2);
    start_panel->y = title_panel->y + title_panel->height + 16;

    help_panel->x = (viewport.w / 2) - (help_panel->width / 2);
    help_panel->y = viewport.y + viewport.h - start_panel->height - 8;

    add_gui_screen(menu_gui_screen);
}

//  ---------------------------------------------------------------------------
void activate_main_menu_game_state(struct Game* game)
{
    if (menu_gui_screen == NULL)
    {
        init_menu_gui_screen();
    }

    activate_gui();
    enable_gui_cursor(0);
    menu_gui_screen->enabled = 1;
}

//  ---------------------------------------------------------------------------
void deactivate_main_menu_game_state(struct Game* game)
{
    deactivate_gui();
    menu_gui_screen->enabled = 0;
}

//  ---------------------------------------------------------------------------
void draw_main_menu_game_state(struct Game* game, int in_transition)
{
    draw_gui();
}

//  ---------------------------------------------------------------------------
void update_main_menu_game_state(struct Game* game)
{
    process_menu_game_state_input(game);
}