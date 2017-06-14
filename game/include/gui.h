#ifndef GUI_HEADER_INCLUDED
#define GUI_HEADER_INCLUDED

struct Game;
struct GuiScreen;

void activate_gui();
void add_gui_screen(struct GuiScreen* screen);
void add_overlay(struct GuiScreen* screen);
void deactivate_gui();
void enable_gui_cursor(const int enabled);
void draw_gui();
void update_gui(const float elapsed_seconds);
void set_gui_cursor_position(const int x, const int y);

#endif