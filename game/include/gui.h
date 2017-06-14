#ifndef GUI_HEADER_INCLUDED
#define GUI_HEADER_INCLUDED

struct Game;
struct GuiScreen;

void activate_gui();
void add_gui_screen(struct GuiScreen* screen);
void add_overlay(struct GuiScreen* screen);
void deactivate_gui();
void enable_gui_cursor(int enabled);
void draw_gui(struct Game* game);
void update_gui(struct Game* game);
void set_gui_cursor_position(int x, int y);

#endif