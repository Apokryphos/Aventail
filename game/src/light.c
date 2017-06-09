#include "actor.h"
#include "actor_list.h"
#include "map.h"
#include "tile.h"
#include "world.h"
#include <assert.h>
#include <stdlib.h>

const int LIGHT_DECAY = 32;

//  ---------------------------------------------------------------------------
static void light_neighbor(
    struct Map* map,
    struct Tile* tile,
    const enum Direction direction,
    int open_cells[],
    int* open_count)
{
    const int light_decay =
        is_diagonal_direction(direction) ?
        (int)(LIGHT_DECAY * 1.41) :
        LIGHT_DECAY;

    struct Tile* neighbor = get_tile_neighbor(map, tile, direction);

    if (neighbor != NULL)
    {
        int neighbor_index = get_map_tile_index(map, neighbor);

        if (index_in_map_bounds(map, neighbor_index))
        {
            struct Tile* neighbor_tile = &map->tiles[neighbor_index];

            //  Ignore void tiles
            if (neighbor_tile->tileset_id < 0)
            {
                return;
            }

            //  Light spreads to less illuminated tiles
            if (neighbor_tile->light < tile->light - light_decay)
            {
                //  Add light to neighbor tile
                neighbor_tile->light = tile->light - light_decay;

                //  If less than or equal to zero than don't add to open list
                if (neighbor_tile->light <= 0)
                {
                    //  Clamp tile light to positive values
                    neighbor_tile->light = 0;
                }
                else
                {
                    //  Add lit neighbor to open list
                    open_cells[*open_count] = neighbor_index;
                    ++(*open_count);
                }
            }
        }
    }
}

//  ---------------------------------------------------------------------------
void update_lighting(struct World* world)
{
    struct Map* map = world->map;
    struct Actor* actor = world->player.actor;
    struct ActorList* actors = world->actors;

    //  Clear existing light levels
    for (int t = 0; t < map->width * map->height; ++t)
    {
        map->tiles[t].light = 0;
    }

    static int open_count = 0;
    static int open_list_size = 0;
    static int* open_cells = NULL;

    if (open_list_size != map->width * map->height)
    {
        if (open_cells != NULL)
        {
            free(open_cells);
        }

        open_cells = malloc(sizeof(int) * map->width * map->height);
    }

    open_count = 1;
    open_cells[0] = actor->tile->y * map->width + actor->tile->x;
    actor->tile->light = 255;

    while (open_count > 0)
    {
        int index = open_cells[0];

        //  Remove from open list by swapping with last index
        open_cells[0] = open_cells[open_count - 1];
        --open_count;

        struct Tile* tile = &map->tiles[index];

        //  Solid tiles can be added to open list but shouldn't continue
        //  to light neighbors
        if (tile->collision == 1)
        {
            continue;
        }

        int light_blocked = 0;

        struct ActorListNode* node = actors->front;
        while (node != NULL)
        {
            //  Check for doors that should block light
            if (node->actor != actor &&
                node->actor->tile == tile &&
                node->actor->type == ACTOR_TYPE_DOOR &&
                node->actor->collision == 1)
            {
                light_blocked = 1;
                break;
            }
            node = node->next;
        }

        if (light_blocked == 0 && tile->light)
        {
            light_neighbor(map, tile, DIRECTION_RIGHT, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_UP, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_LEFT, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_DOWN, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_RIGHT_UP, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_LEFT_UP, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_RIGHT_DOWN, open_cells, &open_count);
            light_neighbor(map, tile, DIRECTION_LEFT_DOWN, open_cells, &open_count);
        }
    }
}