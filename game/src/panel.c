#include "panel.h"
#include <stdlib.h>
#include <string.h>

static const int Style1CornerTilesetId = 1065;
static const int Style1SideTilesetId = 1072;
static const int Style2CornerTilesetId = 1067;
static const int Style2SideTilesetId = 1073;
static const int Style3CornerTilesetId = 1070;

//  ---------------------------------------------------------------------------
struct Panel* CreatePanel(const char* title, enum PanelBorderStyle style)
{
    struct Panel* panel = malloc(sizeof(struct Panel));

    panel->BorderStyle = style;

    panel->Visible = 1;
    panel->Alpha = 255;
    panel->Background = 0;
    panel->Width = 0;
    panel->Height = 0;
    panel->X = 0;
    panel->Y = 0;
    panel->Title = title != NULL ? strdup(title): NULL;
    panel->Text = NULL;
    panel->TextAlign = PANEL_TEXT_ALIGN_LEFT;
    panel->Icon.TilesetId = -1;
    panel->Icon.Flip = 0;
    panel->Icon.Style = PANEL_ICON_STYLE_NONE;

    return panel;
}

//  ---------------------------------------------------------------------------
void DestroyPanel(struct Panel** panel)
{
    if (*panel != NULL)
    {
        if ((*panel)->Title != NULL)
        {
            free((*panel)->Title);
        }

        free(*panel);
        *panel = NULL;
    }
}

//  ---------------------------------------------------------------------------
void GetPanelBorderTilesetIds(
    enum PanelBorderStyle style,
    int* cornerTilesetId,
    int* sideTilesetId)
{
    switch (style)
    {
        default:
            *cornerTilesetId = -1;
            *sideTilesetId = -1;
            break;
        case PANEL_BORDER_STYLE_1:
            *cornerTilesetId = Style1CornerTilesetId;
            *sideTilesetId = Style1SideTilesetId;
            break;
        case PANEL_BORDER_STYLE_2:
            *cornerTilesetId = Style2CornerTilesetId;
            *sideTilesetId = Style2SideTilesetId;
            break;
        case PANEL_BORDER_STYLE_3:
            *cornerTilesetId = Style3CornerTilesetId;
            *sideTilesetId = Style2SideTilesetId;
            break;
    }
}