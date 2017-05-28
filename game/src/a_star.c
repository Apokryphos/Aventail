#include "actor.h"
#include "actor_list.h"
#include "a_star.h"
#include "direction.h"
#include "map.h"
#include "point.h"
#include "tile.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
static void ResetAStar(struct AStar* a_star)
{
    a_star->ClosedCount = 0;
    a_star->OpenCount = 0;

    for (size_t n = 0; n < a_star->NodeCount; ++n)
    {
        struct Tile* tile = &a_star->Map->tiles[n];

        a_star->Nodes[n].F = 0;
        a_star->Nodes[n].G = 0;
        a_star->Nodes[n].H = 0;
        a_star->Nodes[n].Walkable = tile->collision == 0;
        a_star->Nodes[n].Parent = NULL;
        a_star->Nodes[n].X = tile->x;
        a_star->Nodes[n].Y = tile->y;

        a_star->Closed[n] = 0;
        a_star->Open[n] = 0;
    }
}

//  ---------------------------------------------------------------------------
static void UpdateAStar(
    struct AStar* a_star,
    struct Map* map,
    struct ActorList* actor_list)
{
    assert(a_star->Map == map);

    ResetAStar(a_star);

    //  Mark tiles occupied by actors as unwalkable
    struct ActorListNode* actor_node = actor_list->front;
    while (actor_node != NULL)
    {
        struct Tile* tile = actor_node->actor->tile;
        if (tile != NULL)
        {
            size_t index = tile->y * a_star->Map->width + tile->x;
            a_star->Nodes[index].Walkable = 0;
        }

        actor_node = actor_node->next;
    }
}

//  ---------------------------------------------------------------------------
struct AStar* CreateAStar(struct Map* map)
{
    struct AStar* a_star = malloc(sizeof(struct AStar));
    a_star->Map = map;

    a_star->NodeCount = get_map_tile_count(map);
    a_star->Closed = malloc(sizeof(int) * a_star->NodeCount);
    a_star->Open = malloc(sizeof(int) * a_star->NodeCount);
    a_star->Nodes = malloc(sizeof(struct AStarNode) * a_star->NodeCount);

    ResetAStar(a_star);

    return a_star;
}

//  ---------------------------------------------------------------------------
void DestroyAStar(struct AStar** a_star)
{
    if (*a_star != NULL)
    {
        free((*a_star)->Closed);
        free((*a_star)->Open);
        free((*a_star)->Nodes);
        free(*a_star);
        *a_star = NULL;
    }
}

//  ---------------------------------------------------------------------------
static int InClosedList(struct AStar* a_star, size_t nodeIndex)
{
    assert(nodeIndex < a_star->NodeCount);

    for (size_t n = 0; n < a_star->ClosedCount; ++n)
    {
        if (a_star->Closed[n] == nodeIndex)
        {
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
static int InOpenList(struct AStar* a_star, size_t nodeIndex)
{
    assert(nodeIndex < a_star->NodeCount);

    for (size_t n = 0; n < a_star->OpenCount; ++n)
    {
        if (a_star->Open[n] == nodeIndex)
        {
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
/*
    Returns the node index into AStar->Nodes of the lowest F cost node.
*/
static size_t GetLowestCostNodeIndex(struct AStar* a_star)
{
    int cost = INT_MAX;
    size_t index = (size_t) - 1;
    for (size_t n = 0; n < a_star->OpenCount; ++n)
    {
        struct AStarNode* node = &a_star->Nodes[a_star->Open[n]];
        if (node->F < cost)
        {
            //  Node index is the index stored in the open list
            index = a_star->Open[n];
            cost = node->F;
        }
    }

    assert(index < a_star->NodeCount);
    return index;
}

//  ---------------------------------------------------------------------------
static void RemoveFromOpen(struct AStar* a_star, size_t nodeIndex)
{
    assert(a_star->OpenCount > 0);
    assert(nodeIndex < a_star->NodeCount);

    //  Get the index into the open array
    size_t openIndex = (size_t)-1;
    for (size_t n = 0; n < a_star->OpenCount; ++n)
    {
        if (a_star->Open[n] == nodeIndex)
        {
            openIndex = n;
            break;
        }
    }

    assert(openIndex < a_star->OpenCount);

    //  Swap with last array element
    if (openIndex != a_star->OpenCount - 1)
    {
        a_star->Open[openIndex] = a_star->Open[a_star->OpenCount - 1];
    }

    --a_star->OpenCount;
}

//  ---------------------------------------------------------------------------
static int CalculateHCost(struct AStarNode* node1, struct AStarNode* node2)
{
    //  Manhattan distance
    return 10 * abs(node1->X - node2->X) + abs(node1->Y - node2->Y);
}

//  ---------------------------------------------------------------------------
static int CalculateGCost(struct AStarNode* node, struct AStarNode* parentNode)
{
    assert(parentNode != NULL);

    const int cardinalCost = 10;

    int g = cardinalCost + parentNode->G;

    return g;
}

//  ---------------------------------------------------------------------------
struct AStarPath* BuildAStarPath(
    struct AStar* a_star,
    struct Tile* start,
    struct Tile* goal,
    struct Map* map,
    struct ActorList* actor_list)
{
    assert(a_star != NULL);
    assert(start != NULL);
    assert(goal != NULL);
    assert(map != NULL);
    assert(actor_list != NULL);
    assert(start != goal);

    UpdateAStar(a_star, map, actor_list);

    size_t startIndex = get_map_tile_index(a_star->Map, start);
    size_t goalIndex = get_map_tile_index(a_star->Map, goal);

    //  Mark start and goal as walkable or else no path can be found
    a_star->Nodes[startIndex].Walkable = 1;
    a_star->Nodes[goalIndex].Walkable = 1;

    struct AStarNode* goalNode = &a_star->Nodes[goalIndex];

    //  Add start node to open list
    a_star->Open[a_star->OpenCount++] = get_map_tile_index(a_star->Map, start);

    while (a_star->OpenCount > 0)
    {
        size_t currentIndex = GetLowestCostNodeIndex(a_star);

        //  Found goal
        if (currentIndex == goalIndex)
        {
            break;
        }

        RemoveFromOpen(a_star, currentIndex);

        //  Add current node to closed list
        a_star->Closed[a_star->ClosedCount++] = currentIndex;

        struct AStarNode* currentNode = &a_star->Nodes[currentIndex];

        struct Tile* current_tile = &a_star->Map->tiles[currentIndex];

        for (int d = 1; d < 5; ++d)
        {
            enum Direction direction = (enum Direction)d;

            int dx, dy;
            get_direction_delta(direction, &dx, &dy);

            struct Tile* neighborTile = get_map_tile(
                a_star->Map,
                current_tile->x + dx,
                current_tile->y + dy);

            assert(neighborTile != current_tile);

            if (neighborTile != NULL)
            {
                size_t neighborIndex = get_map_tile_index(a_star->Map, neighborTile);
                struct AStarNode* neighborNode = &a_star->Nodes[neighborIndex];

                if (neighborNode->Walkable &&
                    InClosedList(a_star, neighborIndex) == 0)
                {
                    if (InOpenList(a_star, neighborIndex))
                    {
                        //  Check if this cost is lower
                        int gCost = CalculateGCost(neighborNode, currentNode);
                        if (gCost < neighborNode->G)
                        {
                            //  Update parent and cost
                            neighborNode->Parent = currentNode;
                            neighborNode->G = gCost;
                            neighborNode->F = neighborNode->G + neighborNode->H;
                        }
                    }
                    else
                    {
                        //  Add to open list
                        a_star->Open[a_star->OpenCount++] = neighborIndex;
                        neighborNode->Parent = currentNode;
                        neighborNode->G = CalculateGCost(neighborNode, currentNode);
                        neighborNode->H = CalculateHCost(neighborNode, goalNode);
                        neighborNode->F = neighborNode->G + neighborNode->H;
                    }
                }
            }
        }
    }

    if (goalNode->Parent != NULL)
    {
        struct AStarPath* path = malloc(sizeof(struct AStarPath));
        path->Count = 0;

        struct AStarNode* node = goalNode;
        while (node != NULL)
        {
            ++path->Count;
            node = node->Parent;
        }

        path->Points = malloc(sizeof(struct Point) * path->Count);

        size_t p = 0;
        node = goalNode;
        while (node != NULL)
        {
            path->Points[p].X = node->X;
            path->Points[p++].Y = node->Y;

            node = node->Parent;
        }

        //  Reverse path
        size_t start = 0;
        size_t end = path->Count - 1;
        struct Point temp;
        while (start < end)
        {
            temp = path->Points[start];
            path->Points[start] = path->Points[end];
            path->Points[end] = temp;
            ++start;
            --end;
        }

        return path;
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
void DestroyAStarPath(struct AStarPath** path)
{
    if (*path != NULL)
    {
        if ((*path)->Points != NULL)
        {
            free((*path)->Points);
        }
        free(*path);
        *path = NULL;
    }
}

//  ---------------------------------------------------------------------------
struct Point* GetNextPathPoint(struct AStarPath* path, struct Tile* tile)
{
    for (int p = 0; p < path->Count - 1; ++p)
    {
        if (path->Points[p].X == tile->x &&
            path->Points[p].Y == tile->y)
        {
            return &path->Points[p+1];
        }
    }

    return NULL;
}