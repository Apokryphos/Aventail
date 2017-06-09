#ifndef GAME_STATE_MAIN_MENU_HEADER_INCLUDED
#define GAME_STATE_MAIN_MENU_HEADER_INCLUDED

struct Game;

void draw_main_menu_game_state(struct Game* game, int in_transition);
void activate_main_menu_game_state(struct Game* game);
void deactivate_main_menu_game_state(struct Game* game);
void update_main_menu_game_state(struct Game* game);

#endif