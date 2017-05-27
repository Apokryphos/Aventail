#include "blink_state_data.h"
#include "flip_flag.h"
#include "game.h"
#include "gui_screen.h"
#include "panel.h"
#include "render.h"
#include <assert.h>

static int GuiActive = 0;

static float FadeProgress = 0;
static const float FadeDuration = 0.66f;

//  GUI cursor (not the mouse cursor)
static const int CursorTilesetId = 270;

static int CursorEnabled = 0;
static struct BlinkStateData CursorBlink =
{
    .OnDuration = 1.0f,
    .OffDuration = 0.16f,
    .Enabled = 1,
    .Ticks = 0,
    .Visible = 1,
};

static int CursorX = 0;
static int CursorY = 0;

#define GUI_MAX_GUI_SCREENS 16

static int GuiScreenCount = 0;
static struct GuiScreen* GuiScreens[GUI_MAX_GUI_SCREENS];

//  ---------------------------------------------------------------------------
void ActivateGui()
{
    GuiActive = 1;
}

//  ---------------------------------------------------------------------------
void AddGuiScreen(struct GuiScreen* screen)
{
    assert(GuiScreenCount < GUI_MAX_GUI_SCREENS);

    if (GuiScreenCount < GUI_MAX_GUI_SCREENS)
    {
        GuiScreens[GuiScreenCount++] = screen;
    }
}

//  ---------------------------------------------------------------------------
void DeactivateGui()
{
    GuiActive = 0;
}

//  ---------------------------------------------------------------------------
static void DrawCursor(SDL_Renderer* renderer)
{
    if (CursorEnabled && CursorBlink.Visible)
    {
        DrawTilesetTile(renderer, CursorTilesetId, CursorX, CursorY, FLIP_FLAG_NONE);
    }
}

//  ---------------------------------------------------------------------------
static void DrawGuiScreen(SDL_Renderer* renderer, struct GuiScreen* screen)
{
    if (screen->Enabled)
    {
        for (int p = 0; p < screen->PanelCount; ++p)
        {
            //screen->Panels[p]->Alpha = (int)(255 * FadeProgress);
            DrawPanel(renderer, screen->Panels[p]);
        }   
    }
}

//  ---------------------------------------------------------------------------
void EnableCursor(int enable)
{
    CursorEnabled = enable == 0 ? 0 : 1;
}     

//  ---------------------------------------------------------------------------
void GuiDraw(struct Game* game)
{
    if (FadeProgress > 0)
    {
        DrawScreenFade(game->Renderer, 0.9f * FadeProgress);       
    }

    if (GuiActive)
    {
        for (int s = 0; s < GuiScreenCount; ++s)
        {
            DrawGuiScreen(game->Renderer, GuiScreens[s]);
        }

        DrawCursor(game->Renderer);
    }
}

//  ---------------------------------------------------------------------------
void GuiUpdate(struct Game* game)
{
    static float FadeTicks = 0;
    
    if (GuiActive)
    {
        FadeTicks += game->ElapsedSeconds;
        if (FadeTicks > FadeDuration)
        {
            FadeTicks = FadeDuration;
        }

        AddTimeBlinkStateData(&CursorBlink, game->ElapsedSeconds);
    }
    else
    {
        FadeTicks -= game->ElapsedSeconds;
        if (FadeTicks < 0)
        {
            FadeTicks = 0;
        }  
    }

    FadeProgress = (FadeTicks / FadeDuration);
}

//  ---------------------------------------------------------------------------
void SetCursorPosition(int x, int y)
{
    CursorX = x;
    CursorY = y;
} 