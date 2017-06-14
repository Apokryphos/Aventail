#ifndef PLAYER_HUD_HEADER_INCLUDED
#define PLAYER_HUD_HEADER_INCLUDED

struct Game;

void activate_player_hud();
void deactivate_player_hud();
void update_player_hud(const struct Game* game);

#endif