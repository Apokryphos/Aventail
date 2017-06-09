#include "game.h"
#include "game_state.h"
#include "game_state_transition.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
#include "panel.h"
#include "render.h"
#include "world.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdlib.h>

struct GuiScreen* help_gui_screen = NULL;

enum GameState caller_state = GAME_STATE_NONE;

//  ---------------------------------------------------------------------------
void activate_help_game_state(struct Game* game)
{
    assert(caller_state == GAME_STATE_NONE);
    assert(game->state != GAME_STATE_NONE);

    activate_gui();
    enable_gui_cursor(0);
    caller_state = game->state;
    game->state = GAME_STATE_HELP;
}

//  ---------------------------------------------------------------------------
void deactivate_help_game_state(struct Game* game)
{
    deactivate_gui();
    help_gui_screen->enabled = 0;
    caller_state = GAME_STATE_NONE;
}

//  ---------------------------------------------------------------------------
static void process_help_game_state_input(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->accept ||
        input_device->cancel ||
        input_device->help)
    {
        enter_game_state(game, caller_state);
    }
    else if (input_device->inventory)
    {
        enter_game_state(game, GAME_STATE_INVENTORY);
    }
    else if (input_device->gear)
    {
        enter_game_state(game, GAME_STATE_GEAR);
    }
    else if (input_device->status)
    {
        enter_game_state(game, GAME_STATE_STATUS);
    }
}

//  ---------------------------------------------------------------------------
static struct Panel* create_help_panel(const char* text)
{
    int text_width, text_height;
    measure_text(text, &text_width, &text_height);

    struct Panel* panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    panel->text = text;
    panel->width = text_width;
    panel->height = text_height;
    add_panel_to_gui_screen(help_gui_screen, panel);

    return panel;
}

//  ---------------------------------------------------------------------------
static void init_help_gui_screen()
{
    help_gui_screen = create_gui_screen();

    struct Panel* accept_panel = create_help_panel      ("[ENTER] Accept       ");
    struct Panel* cancel_panel = create_help_panel      ("  [ESC] Back / Cancel");
    struct Panel* gear_panel = create_help_panel        ("    [G] Gear         ");
    struct Panel* inventory_panel = create_help_panel   ("    [I] Inventory    ");
    struct Panel* status_panel = create_help_panel      ("    [S] Status       ");
    struct Panel* quit_panel = create_help_panel        ("    [Q] Quit         ");
    struct Panel* move_panel = create_help_panel        ("Use arrow keys to move.");

    //  Add vertical space between sections
    cancel_panel->height *= 2;
    status_panel->height *= 2;
    quit_panel->height *= 2;

    struct Panel* panels[] =
    {
        accept_panel,
        cancel_panel,
        gear_panel,
        inventory_panel,
        status_panel,
        quit_panel,
        move_panel,
    };

    int panel_count = sizeof(panels) / sizeof(struct Panel*);

    stack_panels(panels, panel_count);

    add_gui_screen(help_gui_screen);
}

//  ---------------------------------------------------------------------------
void draw_help_game_state(struct Game* game, int in_transition)
{
    if (game->world->map != NULL)
    {
        draw_map(game->world->map, game->world->actors);
    }

    draw_gui(game);
}

//  ---------------------------------------------------------------------------
void update_help_game_state(struct Game* game)
{
    if (help_gui_screen == NULL)
    {
        init_help_gui_screen();
    }

    help_gui_screen->enabled = 1;

    activate_gui();
    enable_gui_cursor(0);

    process_help_game_state_input(game);
}