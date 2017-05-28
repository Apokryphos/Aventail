#include "gui_screen.h"
#include "panel.h"
#include <assert.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
void add_panel_to_gui_screen(struct GuiScreen* gui_screen, struct Panel* panel)
{
    assert(gui_screen->panel_count < GUI_SCREEN_MAX_PANELS);

    if (gui_screen->panel_count < GUI_SCREEN_MAX_PANELS)
    {
        gui_screen->panels[gui_screen->panel_count++] = panel;
    }
}

//  ---------------------------------------------------------------------------
struct GuiScreen* create_gui_screen()
{
    struct GuiScreen* gui_screen = malloc(sizeof(struct GuiScreen));
    gui_screen->enabled = 0;
    gui_screen->panel_count = 0;
    for (int p = 0; p < GUI_SCREEN_MAX_PANELS; ++p)
    {
        gui_screen->panels[p] = NULL;
    }
    return gui_screen;
}