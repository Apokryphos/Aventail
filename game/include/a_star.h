#ifndef A_STAR_HEADER_INCLUDED
#define A_STAR_HEADER_INCLUDED

#include <stddef.h>

struct ActorList;
struct Map;
struct Tile;

struct AStarPath
{
    int Count;
    struct Point* Points;
};

struct AStarNode;

struct AStarNode
{
    int F;
    int G;
    int H;
    int Walkable;
    int X;
    int Y;
    struct AStarNode* Parent;
};

struct AStar
{
    size_t NodeCount;
    size_t ClosedCount;
    size_t OpenCount;
    int* Closed;
    int* Open;
    struct AStarNode* Nodes;
    struct Map* Map;
};

struct AStar* CreateAStar(struct Map* map);
struct AStarPath* BuildAStarPath(
    struct AStar* aStar,
    struct Tile* start,
    struct Tile* goal,
    struct Map* map,
    struct ActorList* actorList);
void DestroyAStar(struct AStar** aStar);
void DestroyAStarPath(struct AStarPath** path);
/*
    Gets the point in a path after the specified tile's coordinates.
    Returns NULL if the tile's coordinates do not exist in the list
    of path points or if the specified tile's coordinates match the 
    last point in the path.
*/
struct Point* GetNextPathPoint(struct AStarPath* path, struct Tile* tile);

#endif