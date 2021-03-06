#ifndef A_STAR_HEADER_INCLUDED
#define A_STAR_HEADER_INCLUDED

#include <stddef.h>

struct ActorList;
struct Map;
struct Tile;

struct Path
{
    int count;
    struct Point* points;
};

struct PathFinderNode;

struct PathFinder
{
    size_t node_count;
    size_t closed_count;
    size_t open_count;
    int* closed;
    int* open;
    struct PathFinderNode* nodes;
    struct Map* map;
};

struct PathFinder* create_path_finder(struct Map* map);
struct Path* build_path(
    struct PathFinder* path_finder,
    const struct Tile* start,
    const struct Tile* goal,
    const struct Map* map,
    const struct ActorList* actor_list);
void destroy_path_finder(struct PathFinder** path_finder);
void destroy_path(struct Path** path);
/*
    Gets the point in a path after the specified tile's coordinates.
    Returns NULL if the tile's coordinates do not exist in the list
    of path points or if the specified tile's coordinates match the
    last point in the path.
*/
struct Point* get_next_path_point(
    const struct Path* path,
    const struct Tile* tile);

#endif