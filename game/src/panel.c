#include "panel.h"
#include "render.h"
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

static const int PANEL_BORDER_STYLE_1_CORNER_TILESET_ID = 1065;
static const int PANEL_BORDER_STYLE_1_SIDE_TILESET_ID = 1072;
static const int PANEL_BORDER_STYLE_2_CORNER_TILESET_ID = 1067;
static const int PANEL_BORDER_STYLE_2_SIDE_TILESET_ID = 1073;
static const int PANEL_BORDER_STYLE_3_CORNER_TILESET_ID = 1070;

//  ---------------------------------------------------------------------------
struct Panel* create_panel(const char* title, enum PanelBorderStyle style)
{
    struct Panel* panel = malloc(sizeof(struct Panel));

    panel->border_style = style;

    panel->visible = 1;
    panel->alpha = 255;
    panel->background = 0;
    panel->width = 0;
    panel->height = 0;
    panel->show_title = 1;
    panel->x = 0;
    panel->y = 0;
    panel->title = title != NULL ? strdup(title): NULL;
    panel->text = NULL;
    panel->text_align = PANEL_TEXT_ALIGN_LEFT;
    panel->icon.tileset_id = -1;
    panel->icon.flip = 0;
    panel->icon.style = PANEL_ICON_STYLE_NONE;

    return panel;
}

//  ---------------------------------------------------------------------------
void destroy_panel(struct Panel** panel)
{
    if (*panel != NULL)
    {
        if ((*panel)->title != NULL)
        {
            free((*panel)->title);
        }

        free(*panel);
        *panel = NULL;
    }
}

//  ---------------------------------------------------------------------------
void get_panel_border_tileset_ids(
    enum PanelBorderStyle style,
    int* corner_tileset_id,
    int* side_tileset_id)
{
    switch (style)
    {
        default:
            *corner_tileset_id = -1;
            *side_tileset_id = -1;
            break;
        case PANEL_BORDER_STYLE_1:
            *corner_tileset_id = PANEL_BORDER_STYLE_1_CORNER_TILESET_ID;
            *side_tileset_id = PANEL_BORDER_STYLE_1_SIDE_TILESET_ID;
            break;
        case PANEL_BORDER_STYLE_2:
            *corner_tileset_id = PANEL_BORDER_STYLE_2_CORNER_TILESET_ID;
            *side_tileset_id = PANEL_BORDER_STYLE_2_SIDE_TILESET_ID;
            break;
        case PANEL_BORDER_STYLE_3:
            *corner_tileset_id = PANEL_BORDER_STYLE_3_CORNER_TILESET_ID;
            *side_tileset_id = PANEL_BORDER_STYLE_2_SIDE_TILESET_ID;
            break;
    }
}

//  ---------------------------------------------------------------------------
void stack_panels(struct Panel* panels[], const int count)
{
    SDL_Rect viewport;
    get_viewport(&viewport);

    int total_height = 0;

    for (int p = 0; p < count; ++p)
    {
        total_height += panels[p]->height;
    }

    int y = (viewport.h / 2) - (total_height / 2);
    for (int p = 0; p < count; ++p)
    {
        panels[p]->x = (viewport.w / 2) - (panels[p]->width / 2);

        panels[p]->y = y;
        y += panels[p]->height;
    }
}