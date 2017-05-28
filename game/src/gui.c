#include "blink_state_data.h"
#include "flip_flag.h"
#include "game.h"
#include "gui_screen.h"
#include "panel.h"
#include "render.h"
#include <assert.h>

static int gui_active = 0;

static float fade_progress = 0;
static const float FADE_DURATION = 0.66f;

//  GUI cursor (not the mouse cursor)
static const int CURSOR_TILESET_ID = 270;

static int cursor_enabled = 0;
static struct BlinkStateData cursor_blink =
{
    .on_duration = 1.0f,
    .off_duration = 0.16f,
    .enabled = 1,
    .ticks = 0,
    .visible = 1,
};
static int cursor_x = 0;
static int cursor_y = 0;

#define GUI_MAX_GUI_SCREENS 16

static int gui_screen_count = 0;
static struct GuiScreen* gui_screens[GUI_MAX_GUI_SCREENS];

//  ---------------------------------------------------------------------------
void activate_gui()
{
    gui_active = 1;
}

//  ---------------------------------------------------------------------------
void add_gui_screen(struct GuiScreen* screen)
{
    assert(gui_screen_count < GUI_MAX_GUI_SCREENS);

    if (gui_screen_count < GUI_MAX_GUI_SCREENS)
    {
        gui_screens[gui_screen_count++] = screen;
    }
}

//  ---------------------------------------------------------------------------
void deactivate_gui()
{
    gui_active = 0;
}

//  ---------------------------------------------------------------------------
static void draw_gui_cursor(SDL_Renderer* renderer)
{
    if (cursor_enabled && cursor_blink.visible)
    {
        draw_tileset_tile(
            renderer,
            CURSOR_TILESET_ID,
            cursor_x,
            cursor_y,
            FLIP_FLAG_NONE);
    }
}

//  ---------------------------------------------------------------------------
static void draw_gui_screen(SDL_Renderer* renderer, struct GuiScreen* screen)
{
    if (screen->Enabled)
    {
        for (int p = 0; p < screen->PanelCount; ++p)
        {
            //screen->Panels[p]->Alpha = (int)(255 * fade_progress);
            draw_panel(renderer, screen->Panels[p]);
        }
    }
}

//  ---------------------------------------------------------------------------
void enable_gui_cursor(int enable)
{
    cursor_enabled = enable == 0 ? 0 : 1;
}

//  ---------------------------------------------------------------------------
void draw_gui(struct Game* game)
{
    if (fade_progress > 0)
    {
        draw_screen_fade(game->renderer, 0.9f * fade_progress);
    }

    if (gui_active)
    {
        for (int s = 0; s < gui_screen_count; ++s)
        {
            draw_gui_screen(game->renderer, gui_screens[s]);
        }

        draw_gui_cursor(game->renderer);
    }
}

//  ---------------------------------------------------------------------------
void update_gui(struct Game* game)
{
    static float fade_ticks = 0;

    if (gui_active)
    {
        fade_ticks += game->elapsed_seconds;
        if (fade_ticks > FADE_DURATION)
        {
            fade_ticks = FADE_DURATION;
        }

        add_blink_state_data_time(&cursor_blink, game->elapsed_seconds);
    }
    else
    {
        fade_ticks -= game->elapsed_seconds;
        if (fade_ticks < 0)
        {
            fade_ticks = 0;
        }
    }

    fade_progress = (fade_ticks / FADE_DURATION);
}

//  ---------------------------------------------------------------------------
void set_gui_cursor_position(int x, int y)
{
    cursor_x = x;
    cursor_y = y;
}