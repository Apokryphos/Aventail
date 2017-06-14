#ifndef PLAYER_HUD_HEADER_INCLUDED
#define PLAYER_HUD_HEADER_INCLUDED

struct Game;

void activate_player_hud(struct Game* game);
void deactivate_player_hud(struct Game* game);
void update_player_hud(struct Game* game);

#endif