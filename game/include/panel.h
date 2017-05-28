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
    int tileset_id;
    enum PanelIconStyle style;
    enum FlipFlag flip;
};

struct Panel
{
    int visible;
    int background;
    int alpha;
    int width;
    int height;
    int show_title;
    int X;
    int Y;
    struct PanelIcon icon;
    enum PanelBorderStyle border_style;
    enum PanelTextAlign text_align;
    char* title;
    const char* text;
};

struct Panel* create_panel(const char* title, enum PanelBorderStyle style);
void destroy_panel(struct Panel** panel);
void get_panel_border_tileset_ids(
    enum PanelBorderStyle style,
    int* corner_tileset_id,
    int* side_tileset_id);

#endif