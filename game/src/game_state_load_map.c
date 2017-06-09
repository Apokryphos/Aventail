#include "actor.h"
#include "actor_list.h"
#include "audio.h"
#include "game.h"
#include "map.h"
#include "map_link.h"
#include "map_load_util.h"
#include "paths.h"
#include "render.h"
#include "world.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static struct MapLink* dest_link = NULL;
static char* dest_map_name = NULL;

//  ---------------------------------------------------------------------------
static void load_map(const char* asset_filename, struct World* world)
{
    assert(asset_filename != NULL);
    assert(world != NULL);

    //  Remove player from actors so it doesn't get freed
    remove_actor_from_actor_list(world->actors, world->player.actor);

    destroy_actor_list(&world->actors);
    destroy_map(&world->map);

    char *full_path = create_map_file_path(asset_filename);
    FILE *file = fopen(full_path, "rb");

    if (file == NULL)
    {
        perror("");
        fprintf(stderr, "Failed to open map file '%s'.\n", full_path);
        free(full_path);
        exit(EXIT_FAILURE);
    }

    if (load_map_from_file(file, &world->map, &world->actors) != 0)
    {
        fprintf(stderr, "Failed to load map file '%s'.\n", full_path);
        fclose(file);
        free(full_path);
        exit(EXIT_FAILURE);
    }

    //  Add player actor back in
    add_actor_to_actor_list_back(world->actors, world->player.actor);

    free(full_path);
    fclose(file);
}

//  ---------------------------------------------------------------------------
static void load_map_link(struct Game* game)
{
    assert(dest_link != NULL);
    assert(dest_link->dest_map != NULL);

    //  Copy destination link data before destroy_map frees it
    char* dest_map = strdup(dest_link->dest_map);
    int dest_x = dest_link->dest_x;
    int dest_y = dest_link->dest_y;

    dest_link = NULL;

    struct World* world = game->world;

    load_map(dest_map, world);

    free(dest_map);

    struct Tile* dest_tile = get_map_tile(world->map, dest_x, dest_y);
    assert(dest_tile != NULL);
    assert(in_map_bounds(world->map, dest_x, dest_y));

    world->player.actor->map = world->map;
    world->player.actor->tile = dest_tile;

    play_sfx(SFX_STEPS_ENTER);
}

//  ---------------------------------------------------------------------------
void activate_load_map_game_state(struct Game* game)
{
    if (dest_link != NULL)
    {
        load_map_link(game);
    }
    else
    {
        load_map(dest_map_name, game->world);

        free(dest_map_name);
        dest_map_name = NULL;

        game->world->player.actor->map = game->world->map;

        game->world->player.actor->tile =
            get_map_tile(game->world->map, 12, 10);
    }
}

//  ---------------------------------------------------------------------------
void deactivate_load_map_game_state(struct Game* game)
{
}

//  ---------------------------------------------------------------------------
void draw_load_map_game_state(struct Game* game, int in_transition)
{
    if (game->world->map != NULL)
    {
        draw_map(game->world);
    }
}

//  ---------------------------------------------------------------------------
void queue_map_link_load(struct MapLink* link)
{
    dest_link = link;
    play_sfx(SFX_STEPS_EXIT);
}

//  ---------------------------------------------------------------------------
void queue_map_load(const char* map_name)
{
    dest_map_name = strdup(map_name);
    dest_link = NULL;
}

//  ---------------------------------------------------------------------------
void update_load_map_game_state(struct Game* game)
{
    enter_game_state(game, GAME_STATE_LEVEL);
}