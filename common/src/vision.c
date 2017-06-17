#include "actor.h"
#include "actor_ai.h"
#include "actor_list.h"
#include "map.h"
#include "tile.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

static const int HIDDEN_VISION_CELL = 0;
static const int VISIBLE_VISION_CELL = 1;
static const int SOLID_VISION_CELL = 2;

struct VisionMap
{
    int height;
    int width;
    int* cells;
};

//  ---------------------------------------------------------------------------
// static int in_vision_map_bounds(
//     const struct VisionMap* vision_map,
//     const int x,
//     const int y)
// {
//     assert(vision_map != NULL);
//     return
//         x >= 0 &&
//         y >= 0 &&
//         x < vision_map->width &&
//         y < vision_map->height;
// }

//  ---------------------------------------------------------------------------
static void fire_line(
    struct VisionMap* vision_map,
    int x,
    int y,
    int x2,
    int y2)
{
    int w = x2 - x;
    int h = y2 - y;
    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

    if (w < 0)
    {
        dx1 = -1;
    }
    else if (w > 0)
    {
        dx1 = 1;
    }

    if (h < 0)
    {
        dy1 = -1;
    }
    else if (h > 0)
    {
        dy1 = 1;
    }

    if (w < 0)
    {
        dx2 = -1;
    }
    else if (w > 0)
    {
        dx2 = 1;
    }

    int longest = abs(w);
    int shortest = abs(h);
    if (!(longest > shortest))
    {
        longest = abs(h);
        shortest = abs(w);

        if (h < 0)
        {
            dy2 = -1;
        }
        else if (h > 0)
        {
            dy2 = 1;
        }

        dx2 = 0;
    }
    int numerator = longest >> 1;
    for (int i = 0; i <= longest; i++)
    {
        //  Bounds check: not required
        // if (in_vision_map_bounds(vision_map, x, y) == 0)
        // {
        //     return;
        // }

        if (vision_map->cells[y * vision_map->width + x] == SOLID_VISION_CELL)
        {
            return;
        }

        vision_map->cells[y * vision_map->width + x] = VISIBLE_VISION_CELL;

        numerator += shortest;
        if (!(numerator < longest))
        {
            numerator -= longest;
            x += dx1;
            y += dy1;
        }
        else
        {
            x += dx2;
            y += dy2;
        }
    }
}

//  ---------------------------------------------------------------------------
static struct VisionMap* create_vision_map(const int width, const int height)
{
    struct VisionMap* vision_map = malloc(sizeof(struct ActorAi));

    vision_map->width = width;
    vision_map->height = height;

    vision_map->cells = malloc(sizeof(int) * width * height);

    return vision_map;
}

//  ---------------------------------------------------------------------------
void destroy_vision_map(struct VisionMap** vision_map)
{
    if (*vision_map != NULL)
    {
        free((*vision_map)->cells);
        free(*vision_map);
        *vision_map = NULL;
    }
}

//  ---------------------------------------------------------------------------
static void update_vision_map(
    struct VisionMap* vision_map,
    const struct Map* map,
    const struct ActorList* actors,
    const int pos_x,
    const int pos_y)
{
    assert(map->width == vision_map->width);
    assert(map->height == vision_map->height);

    //  Mark all solid map tiles as solid vision cells
    for (int c = 0; c < vision_map->width * vision_map->height; ++c)
    {
        vision_map->cells[c] =
            map->tiles[c].collision ?
            SOLID_VISION_CELL :
            HIDDEN_VISION_CELL;
    }

    //  Mark all door actor tiles as solid vision cells
    struct ActorListNode* node = actors->front;
    while (node != NULL)
    {
        struct Actor* actor = node->actor;
        if (actor->type == ACTOR_TYPE_DOOR &&
            actor->collision != 0)
        {
            struct Tile* tile = actor->tile;
            vision_map->cells[tile->y * vision_map->width + tile->x] = SOLID_VISION_CELL;
        }
        node = node->next;
    }

    for (int x = 0; x < vision_map->width; ++x)
    {
        fire_line(vision_map, pos_x, pos_y, x, 0);
    }

    for (int x = 0; x < vision_map->width; ++x)
    {
        fire_line(vision_map, pos_x, pos_y, x, vision_map->height - 1);
    }

    for (int y = 0; y < vision_map->height; ++y)
    {
        fire_line(vision_map, pos_x, pos_y, 0, y);
    }

    for (int y = 0; y < vision_map->height; ++y)
    {
        fire_line(vision_map, pos_x, pos_y, vision_map->width - 1, y);
    }
}

//  ---------------------------------------------------------------------------
void update_vision(
    const struct Map* map,
    struct Actor* source,
    const struct ActorList* actors)
{
    assert(map != NULL);
    assert(source != NULL);
    assert(actors != NULL);

    if (source->vision_map == NULL ||
        source->vision_map->width != map->width ||
        source->vision_map->height != map->height)
    {
        destroy_vision_map(&source->vision_map);
        source->vision_map = create_vision_map(map->width, map->height);
    }

    if (source->tile != NULL)
    {
        update_vision_map(
            source->vision_map,
            map,
            actors,
            source->tile->x,
            source->tile->y);
    }
}

//  ---------------------------------------------------------------------------
int can_see_actor(
    const struct Map* map,
    const struct Actor* source,
    const struct Actor* target,
    const struct ActorList* actors)
{
    assert(map != NULL);
    assert(source != NULL);
    assert(target != NULL);
    assert(actors != NULL);

    const struct VisionMap* vision_map = source->vision_map;

    assert(vision_map != NULL);
    assert(vision_map->width == map->width);
    assert(vision_map->height == map->height);

    if (source->tile != NULL &&
        target->tile != NULL)
    {
        if (vision_map->cells[target->tile->y * vision_map->width + target->tile->x] == VISIBLE_VISION_CELL)
        {
            return 1;
        }
    }

    return 0;
}