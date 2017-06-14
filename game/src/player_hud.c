#include "actor.h"
#include "game.h"
#include "gui.h"
#include "gui_gauge_string.h"
#include "gui_screen.h"
#include "panel.h"
#include "world.h"
#include <stdlib.h>

static struct GuiScreen* player_hud_gui_screen = NULL;

static struct GuiGaugeString* health_gauge_string = NULL;

static struct Panel* health_panel = NULL;

//  ---------------------------------------------------------------------------
static void init_player_hud_gui_screen()
{
    player_hud_gui_screen = create_gui_screen();

    health_panel = create_panel("", PANEL_BORDER_STYLE_NONE);

    health_gauge_string = create_gui_gauge_string();

    add_panel_to_gui_screen(player_hud_gui_screen, health_panel);
    add_overlay(player_hud_gui_screen);
}

//  ---------------------------------------------------------------------------
void activate_player_hud(struct Game* game)
{
    if (player_hud_gui_screen == NULL)
    {
        init_player_hud_gui_screen();
    }

    player_hud_gui_screen->enabled = 1;
}

//  ---------------------------------------------------------------------------
void deactivate_player_hud(struct Game* game)
{
    if (player_hud_gui_screen == NULL)
    {
        init_player_hud_gui_screen();
    }

    player_hud_gui_screen->enabled = 0;
}

//  ---------------------------------------------------------------------------
void update_player_hud(struct Game* game)
{
    if (player_hud_gui_screen == NULL)
    {
        init_player_hud_gui_screen();
    }

    struct Actor* player_actor = game->world->player.actor;

    set_gui_gauge_string(
        health_gauge_string,
        "Health",
        player_actor->health,
        player_actor->max_health);

    health_panel->text = health_gauge_string->string;
}