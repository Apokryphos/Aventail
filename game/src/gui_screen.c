#include "gui_screen.h"
#include "panel.h"
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
void AddGuiScreenPanel(struct GuiScreen* screen, struct Panel* panel)
{
    assert(screen->PanelCount < GUI_SCREEN_MAX_PANELS);

    if (screen->PanelCount < GUI_SCREEN_MAX_PANELS)
    {
        screen->Panels[screen->PanelCount++] = panel;
    }
}

//  ---------------------------------------------------------------------------
struct GuiScreen* CreateGuiScreen()
{
    struct GuiScreen* screen = malloc(sizeof(struct GuiScreen));
    screen->Enabled = 0;
    screen->PanelCount = 0;
    for (int p = 0; p < GUI_SCREEN_MAX_PANELS; ++p)
    {
        screen->Panels[p] = NULL;
    }
    return screen;
}