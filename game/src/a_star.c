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
static void ResetAStar(struct AStar* aStar)
{
    aStar->ClosedCount = 0;
    aStar->OpenCount = 0;

    for (size_t n = 0; n < aStar->NodeCount; ++n)
    {
        struct Tile* tile = &aStar->Map->Tiles[n];

        aStar->Nodes[n].F = 0;
        aStar->Nodes[n].G = 0;
        aStar->Nodes[n].H = 0;
        aStar->Nodes[n].Walkable = tile->Collision == 0;
        aStar->Nodes[n].Parent = NULL;
        aStar->Nodes[n].X = tile->X;
        aStar->Nodes[n].Y = tile->Y;

        aStar->Closed[n] = 0;
        aStar->Open[n] = 0;
    }
}

//  ---------------------------------------------------------------------------
static void UpdateAStar(
    struct AStar* aStar,
    struct Map* map,
    struct ActorList* actorList)
{
    assert(aStar->Map == map);

    ResetAStar(aStar);

    //  Mark tiles occupied by actors as unwalkable
    struct ActorListNode* actorNode = actorList->First;
    while (actorNode != NULL)
    {
        struct Tile* tile = actorNode->Actor->tile;
        if (tile != NULL)
        {
            size_t index = tile->Y * aStar->Map->Width + tile->X;
            aStar->Nodes[index].Walkable = 0;
        }

        actorNode = actorNode->Next;
    }
}

//  ---------------------------------------------------------------------------
struct AStar* CreateAStar(struct Map* map)
{
    struct AStar* aStar = malloc(sizeof(struct AStar));
    aStar->Map = map;

    aStar->NodeCount = GetTileCount(map);
    aStar->Closed = malloc(sizeof(int) * aStar->NodeCount);
    aStar->Open = malloc(sizeof(int) * aStar->NodeCount);
    aStar->Nodes = malloc(sizeof(struct AStarNode) * aStar->NodeCount);

    ResetAStar(aStar);

    return aStar;
}

//  ---------------------------------------------------------------------------
void DestroyAStar(struct AStar** aStar)
{
    if (*aStar != NULL)
    {
        free((*aStar)->Closed);
        free((*aStar)->Open);
        free((*aStar)->Nodes);
        free(*aStar);
        *aStar = NULL;
    }
}

//  ---------------------------------------------------------------------------
static int InClosedList(struct AStar* aStar, size_t nodeIndex)
{
    assert(nodeIndex < aStar->NodeCount);

    for (size_t n = 0; n < aStar->ClosedCount; ++n)
    {
        if (aStar->Closed[n] == nodeIndex)
        {
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
static int InOpenList(struct AStar* aStar, size_t nodeIndex)
{
    assert(nodeIndex < aStar->NodeCount);

    for (size_t n = 0; n < aStar->OpenCount; ++n)
    {
        if (aStar->Open[n] == nodeIndex)
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
static size_t GetLowestCostNodeIndex(struct AStar* aStar)
{
    int cost = INT_MAX;
    size_t index = (size_t) - 1;
    for (size_t n = 0; n < aStar->OpenCount; ++n)
    {
        struct AStarNode* node = &aStar->Nodes[aStar->Open[n]];
        if (node->F < cost)
        {
            //  Node index is the index stored in the open list
            index = aStar->Open[n];
            cost = node->F;
        }
    }

    assert(index < aStar->NodeCount);
    return index;
}

//  ---------------------------------------------------------------------------
static void RemoveFromOpen(struct AStar* aStar, size_t nodeIndex)
{
    assert(aStar->OpenCount > 0);
    assert(nodeIndex < aStar->NodeCount);

    //  Get the index into the open array
    size_t openIndex = (size_t)-1;
    for (size_t n = 0; n < aStar->OpenCount; ++n)
    {
        if (aStar->Open[n] == nodeIndex)
        {
            openIndex = n;
            break;
        }
    }

    assert(openIndex < aStar->OpenCount);

    //  Swap with last array element
    if (openIndex != aStar->OpenCount - 1)
    {
        aStar->Open[openIndex] = aStar->Open[aStar->OpenCount - 1];
    }

    --aStar->OpenCount;
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
    struct AStar* aStar,
    struct Tile* start,
    struct Tile* goal,
    struct Map* map,
    struct ActorList* actorList)
{
    assert(aStar != NULL);
    assert(start != NULL);
    assert(goal != NULL);
    assert(map != NULL);
    assert(actorList != NULL);
    assert(start != goal);

    UpdateAStar(aStar, map, actorList);

    size_t startIndex = GetTileIndex(aStar->Map, start);
    size_t goalIndex = GetTileIndex(aStar->Map, goal);

    //  Mark start and goal as walkable or else no path can be found
    aStar->Nodes[startIndex].Walkable = 1;
    aStar->Nodes[goalIndex].Walkable = 1;

    struct AStarNode* goalNode = &aStar->Nodes[goalIndex];

    //  Add start node to open list
    aStar->Open[aStar->OpenCount++] = GetTileIndex(aStar->Map, start);

    while (aStar->OpenCount > 0)
    {
        size_t currentIndex = GetLowestCostNodeIndex(aStar);

        //  Found goal
        if (currentIndex == goalIndex)
        {
            break;
        }

        RemoveFromOpen(aStar, currentIndex);

        //  Add current node to closed list
        aStar->Closed[aStar->ClosedCount++] = currentIndex;

        struct AStarNode* currentNode = &aStar->Nodes[currentIndex];

        struct Tile* currentTile = &aStar->Map->Tiles[currentIndex];

        for (int d = 1; d < 5; ++d)
        {
            enum Direction direction = (enum Direction)d;

            int dx, dy;
            GetDelta(direction, &dx, &dy);

            struct Tile* neighborTile = GetTile(
                aStar->Map,
                currentTile->X + dx,
                currentTile->Y + dy);

            assert(neighborTile != currentTile);

            if (neighborTile != NULL)
            {
                size_t neighborIndex = GetTileIndex(aStar->Map, neighborTile);
                struct AStarNode* neighborNode = &aStar->Nodes[neighborIndex];

                if (neighborNode->Walkable &&
                    InClosedList(aStar, neighborIndex) == 0)
                {
                    if (InOpenList(aStar, neighborIndex))
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
                        aStar->Open[aStar->OpenCount++] = neighborIndex;
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
        if (path->Points[p].X == tile->X &&
            path->Points[p].Y == tile->Y)
        {
            return &path->Points[p+1];
        }
    }

    return NULL;
}