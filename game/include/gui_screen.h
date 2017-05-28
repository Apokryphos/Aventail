#ifndef GUI_SCREEN_HEADER_INCLUDED
#define GUI_SCREEN_HEADER_INCLUDED

#define GUI_SCREEN_MAX_PANELS 100

struct Panel;

struct GuiScreen
{
    int enabled;
    int panel_count;
    struct Panel* panels[GUI_SCREEN_MAX_PANELS];
};

void add_panel_to_gui_screen(struct GuiScreen* gui_screen, struct Panel* panel);
struct GuiScreen* create_gui_screen();

#endif