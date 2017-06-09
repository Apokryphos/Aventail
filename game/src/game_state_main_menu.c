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
        queue_map_load("map01");
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

    const char* titleText = "Aventail";
    int titleWidth, titleHeight;
    measure_text(titleText, &titleWidth, &titleHeight);

    struct Panel* titlePanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    titlePanel->text = titleText;
    titlePanel->width = titleWidth;
    titlePanel->height = titleHeight;
    add_panel_to_gui_screen(menu_gui_screen, titlePanel);

    const char* startText = "Press Enter to start.";
    int startWidth, startHeight;
    measure_text(startText, &startWidth, &startHeight);

    struct Panel* startPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    startPanel->text = startText;
    startPanel->width = startWidth;
    startPanel->height = startHeight;
    add_panel_to_gui_screen(menu_gui_screen, startPanel);

    const char* helpText = "Press ? at any time for help.";
    int helpWidth, helpHeight;
    measure_text(helpText, &helpWidth, &helpHeight);

    struct Panel* helpPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    helpPanel->text = helpText;
    helpPanel->width = helpWidth;
    helpPanel->height = helpHeight;
    add_panel_to_gui_screen(menu_gui_screen, helpPanel);

    //  Center title
    SDL_Rect viewport;
    get_viewport(&viewport);
    titlePanel->x = (viewport.w / 2) - (titlePanel->width / 2);
    titlePanel->y = (viewport.h / 2) - (titlePanel->height / 2);

    startPanel->x = (viewport.w / 2) - (startPanel->width / 2);
    startPanel->y = titlePanel->y + titlePanel->height + 16;

    helpPanel->x = (viewport.w / 2) - (helpPanel->width / 2);
    helpPanel->y = viewport.y + viewport.h - startPanel->height - 8;

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
    draw_gui(game);
}

//  ---------------------------------------------------------------------------
void update_main_menu_game_state(struct Game* game)
{
    process_menu_game_state_input(game);
}