#include "actor.h"
#include "actor_list.h"
#include "path_finder.h"
#include "direction.h"
#include "map.h"
#include "point.h"
#include "tile.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

//  ---------------------------------------------------------------------------
static void reset_path_finder(struct PathFinder* path_finder)
{
    path_finder->closed_count = 0;
    path_finder->open_count = 0;

    for (size_t n = 0; n < path_finder->node_count; ++n)
    {
        struct Tile* tile = &path_finder->map->tiles[n];

        path_finder->nodes[n].f_cost = 0;
        path_finder->nodes[n].g_cost = 0;
        path_finder->nodes[n].h_cost = 0;
        path_finder->nodes[n].walkable = tile->collision == 0;
        path_finder->nodes[n].parent = NULL;
        path_finder->nodes[n].x = tile->x;
        path_finder->nodes[n].y = tile->y;

        path_finder->closed[n] = 0;
        path_finder->open[n] = 0;
    }
}

//  ---------------------------------------------------------------------------
static void update_path_finder(
    struct PathFinder* path_finder,
    const struct Map* map,
    const struct ActorList* actor_list)
{
    assert(path_finder->map == map);

    reset_path_finder(path_finder);

    //  Mark tiles occupied by actors as unwalkable
    const struct ActorListNode* actor_node = actor_list->front;
    while (actor_node != NULL)
    {
        //  Ignore doors and dead actors
        if (actor_node->actor->type != ACTOR_TYPE_DOOR &&
            is_actor_dead(actor_node->actor) == 0)
        {
            const struct Tile* tile = actor_node->actor->tile;
            if (tile != NULL)
            {
                size_t index = tile->y * path_finder->map->width + tile->x;
                path_finder->nodes[index].walkable = 0;
            }
        }

        actor_node = actor_node->next;
    }
}

//  ---------------------------------------------------------------------------
struct PathFinder* create_path_finder(struct Map* map)
{
    struct PathFinder* path_finder = malloc(sizeof(struct PathFinder));
    path_finder->map = map;

    path_finder->node_count = get_map_tile_count(map);
    path_finder->closed = malloc(sizeof(int) * path_finder->node_count);
    path_finder->open = malloc(sizeof(int) * path_finder->node_count);
    path_finder->nodes = malloc(sizeof(struct PathFinderNode) * path_finder->node_count);

    reset_path_finder(path_finder);

    return path_finder;
}

//  ---------------------------------------------------------------------------
void destroy_path_finder(struct PathFinder** path_finder)
{
    if (*path_finder != NULL)
    {
        free((*path_finder)->closed);
        free((*path_finder)->open);
        free((*path_finder)->nodes);
        free(*path_finder);
        *path_finder = NULL;
    }
}

//  ---------------------------------------------------------------------------
static int closed_list_contains_node_index(
    const struct PathFinder* path_finder,
    const size_t node_index)
{
    assert(node_index < path_finder->node_count);

    for (size_t n = 0; n < path_finder->closed_count; ++n)
    {
        if (path_finder->closed[n] == node_index)
        {
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
static int open_list_contains_node_index(
    const struct PathFinder* path_finder,
    const size_t node_index)
{
    assert(node_index < path_finder->node_count);

    for (size_t n = 0; n < path_finder->open_count; ++n)
    {
        if (path_finder->open[n] == node_index)
        {
            return 1;
        }
    }

    return 0;
}

//  ---------------------------------------------------------------------------
/*
    Returns the node index into PathFinder->nodes of the lowest F cost node.
*/
static size_t get_lowest_cost_node_index(const struct PathFinder* path_finder)
{
    int cost = INT_MAX;
    size_t index = (size_t) - 1;
    for (size_t n = 0; n < path_finder->open_count; ++n)
    {
        const struct PathFinderNode* node = &path_finder->nodes[path_finder->open[n]];
        if (node->f_cost < cost)
        {
            //  Node index is the index stored in the open list
            index = path_finder->open[n];
            cost = node->f_cost;
        }
    }

    assert(index < path_finder->node_count);
    return index;
}

//  ---------------------------------------------------------------------------
static void removed_node_index_from_open_list(
    struct PathFinder* path_finder,
    size_t node_index)
{
    assert(path_finder->open_count > 0);
    assert(node_index < path_finder->node_count);

    //  Get the index into the open array
    size_t open_index = (size_t)-1;
    for (size_t n = 0; n < path_finder->open_count; ++n)
    {
        if (path_finder->open[n] == node_index)
        {
            open_index = n;
            break;
        }
    }

    assert(open_index < path_finder->open_count);

    //  Swap with last array element
    if (open_index != path_finder->open_count - 1)
    {
        path_finder->open[open_index] = path_finder->open[path_finder->open_count - 1];
    }

    --path_finder->open_count;
}

//  ---------------------------------------------------------------------------
static int calculate_h_cost(
    const struct PathFinderNode* node1,
    const struct PathFinderNode* node2)
{
    //  Manhattan distance
    return 10 * abs(node1->x - node2->x) + abs(node1->y - node2->y);
}

//  ---------------------------------------------------------------------------
static int calculate_g_cost(
    const struct PathFinderNode* node,
    const struct PathFinderNode* parentNode)
{
    assert(parentNode != NULL);

    const int cardinalCost = 10;

    int g = cardinalCost + parentNode->g_cost;

    return g;
}

//  ---------------------------------------------------------------------------
struct Path* build_path(
    struct PathFinder* path_finder,
    const struct Tile* start,
    const struct Tile* goal,
    const struct Map* map,
    const struct ActorList* actor_list)
{
    assert(path_finder != NULL);
    assert(start != NULL);
    assert(goal != NULL);
    assert(map != NULL);
    assert(actor_list != NULL);
    assert(start != goal);

    update_path_finder(path_finder, map, actor_list);

    size_t start_index = get_map_tile_index(path_finder->map, start);
    size_t goal_index = get_map_tile_index(path_finder->map, goal);

    //  Mark start and goal as walkable or else no path can be found
    path_finder->nodes[start_index].walkable = 1;
    path_finder->nodes[goal_index].walkable = 1;

    struct PathFinderNode* goal_node = &path_finder->nodes[goal_index];

    //  Add start node to open list
    path_finder->open[path_finder->open_count++] =
        get_map_tile_index(path_finder->map, start);

    while (path_finder->open_count > 0)
    {
        size_t current_index = get_lowest_cost_node_index(path_finder);

        //  Found goal
        if (current_index == goal_index)
        {
            break;
        }

        removed_node_index_from_open_list(path_finder, current_index);

        //  Add current node to closed list
        path_finder->closed[path_finder->closed_count++] = current_index;

        struct PathFinderNode* current_node = &path_finder->nodes[current_index];

        const struct Tile* current_tile = &path_finder->map->tiles[current_index];

        for (int d = 1; d < 5; ++d)
        {
            enum Direction direction = (enum Direction)d;

            int dx, dy;
            get_direction_delta(direction, &dx, &dy);

            const struct Tile* neighbor_tile = get_map_tile(
                path_finder->map,
                current_tile->x + dx,
                current_tile->y + dy);

            assert(neighbor_tile != current_tile);

            if (neighbor_tile != NULL)
            {
                size_t neighbor_index =
                    get_map_tile_index(path_finder->map, neighbor_tile);

                struct PathFinderNode* neighbor_node =
                    &path_finder->nodes[neighbor_index];

                if (neighbor_node->walkable &&
                    closed_list_contains_node_index(path_finder, neighbor_index) == 0)
                {
                    if (open_list_contains_node_index(path_finder, neighbor_index))
                    {
                        //  Check if this cost is lower
                        int gCost = calculate_g_cost(neighbor_node, current_node);
                        if (gCost < neighbor_node->g_cost)
                        {
                            //  Update parent and cost
                            neighbor_node->parent = current_node;
                            neighbor_node->g_cost = gCost;
                            neighbor_node->f_cost = neighbor_node->g_cost + neighbor_node->h_cost;
                        }
                    }
                    else
                    {
                        //  Add to open list
                        path_finder->open[path_finder->open_count++] = neighbor_index;
                        neighbor_node->parent = current_node;
                        neighbor_node->g_cost = calculate_g_cost(neighbor_node, current_node);
                        neighbor_node->h_cost = calculate_h_cost(neighbor_node, goal_node);
                        neighbor_node->f_cost = neighbor_node->g_cost + neighbor_node->h_cost;
                    }
                }
            }
        }
    }

    if (goal_node->parent != NULL)
    {
        struct Path* path = malloc(sizeof(struct Path));
        path->count = 0;

        struct PathFinderNode* node = goal_node;
        while (node != NULL)
        {
            ++path->count;
            node = node->parent;
        }

        path->points = malloc(sizeof(struct Point) * path->count);

        size_t p = 0;
        node = goal_node;
        while (node != NULL)
        {
            path->points[p].x = node->x;
            path->points[p++].y = node->y;

            node = node->parent;
        }

        //  Reverse path
        size_t start = 0;
        size_t end = path->count - 1;
        struct Point temp;
        while (start < end)
        {
            temp = path->points[start];
            path->points[start] = path->points[end];
            path->points[end] = temp;
            ++start;
            --end;
        }

        return path;
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
void destroy_path(struct Path** path)
{
    if (*path != NULL)
    {
        if ((*path)->points != NULL)
        {
            free((*path)->points);
        }
        free(*path);
        *path = NULL;
    }
}

//  ---------------------------------------------------------------------------
struct Point* get_next_path_point(
    const struct Path* path,
    const struct Tile* tile)
{
    for (int p = 0; p < path->count - 1; ++p)
    {
        if (path->points[p].x == tile->x &&
            path->points[p].y == tile->y)
        {
            return &path->points[p+1];
        }
    }

    return NULL;
}