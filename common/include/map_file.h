#ifndef MAP_FILE_HEADER_INCLUDED
#define MAP_FILE_HEADER_INCLUDED

struct Actor;
struct Tile;

extern const int ERROR_MAP_FILE_LOAD;

extern const int MAP_FILE_STRING_MAX_LENGTH;

void dump_actor_info(const struct Actor* actor);

void dump_map_link_info(const struct Tile* tile);

int validate_map_file_map_link_count(const int count);

int validate_map_file_string_len(const int len);

#endif