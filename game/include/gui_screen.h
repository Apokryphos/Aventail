#ifndef GUI_SCREEN_HEADER_INCLUDED
#define GUI_SCREEN_HEADER_INCLUDED

#define GUI_SCREEN_MAX_PANELS 100

struct Panel;

struct GuiScreen
{
    int Enabled;
    int PanelCount;
    struct Panel* Panels[GUI_SCREEN_MAX_PANELS];
};

void AddGuiScreenPanel(struct GuiScreen* screen, struct Panel* panel);
struct GuiScreen* CreateGuiScreen();

#endif