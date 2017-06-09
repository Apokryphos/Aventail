#ifndef GAME_STATE_LOAD_LEVEL_HEADER_INCLUDED
#define GAME_STATE_LOAD_LEVEL_HEADER_INCLUDED

struct Game;
struct MapLink;

void activate_load_map_game_state(struct Game* game);
void deactivate_load_map_game_state(struct Game* game);
void draw_load_map_game_state(struct Game* game, int in_transition);
void queue_map_link_load(struct MapLink* link);
void queue_map_load(const char* map_name);
void update_load_map_game_state(struct Game* game);

#endif