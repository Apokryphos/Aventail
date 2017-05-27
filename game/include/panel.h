#ifndef PANEL_HEADER_DEFINED
#define PANEL_HEADER_DEFINED

#include "flip_flag.h"

enum PanelBorderStyle
{
    PANEL_BORDER_STYLE_NONE,
    PANEL_BORDER_STYLE_1,
    PANEL_BORDER_STYLE_2,
    PANEL_BORDER_STYLE_3,
};

enum PanelIconStyle
{
    PANEL_ICON_STYLE_NONE,
    PANEL_ICON_STYLE_NORMAL,
    PANEL_ICON_STYLE_SMALL,
};

enum PanelTextAlign
{
    PANEL_TEXT_ALIGN_LEFT,
    PANEL_TEXT_ALIGN_CENTER,
};

struct PanelIcon
{
    int TilesetId;
    enum PanelIconStyle Style;
    enum FlipFlag Flip;
};

struct Panel
{
    int Visible;
    int Background;
    int Alpha;
    int Width;
    int Height;
    int X;
    int Y;
    struct PanelIcon Icon;
    enum PanelBorderStyle BorderStyle;
    enum PanelTextAlign TextAlign;
    char* Title;
    const char* Text;
};

struct Panel* CreatePanel(const char* title, enum PanelBorderStyle style);
void DestroyPanel(struct Panel** panel);
void GetPanelBorderTilesetIds(
    enum PanelBorderStyle style,
    int* cornerTilesetId,
    int* sideTilesetId);

#endif