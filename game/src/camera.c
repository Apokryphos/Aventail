#include "actor.h"
#include "camera.h"
#include "game.h"
#include "map.h"
#include "player.h"
#include "tile.h"
#include "world.h"
#include <math.h>

//  ---------------------------------------------------------------------------
void set_camera_position(
    struct Camera* camera,
    const int tile_x,
    const int tile_y)
{
    camera->tile_x = tile_x;
    camera->tile_y = tile_y;
    camera->offset_x = 0;
    camera->offset_y = 0;
}

//  ---------------------------------------------------------------------------
void update_camera(
    struct Camera* camera,
    const int target_tile_x,
    const int target_tile_y,
    const int tile_width,
    const int tile_height,
    const float elapsed_seconds)
{
    int dx = target_tile_x - camera->tile_x;
    int dy = target_tile_y - camera->tile_y;

    if (dx > 0)
    {
        camera->offset_x += camera->speed * dx * elapsed_seconds;

        if (camera->offset_x >= tile_width)
        {
            camera->offset_x = 0;
            ++camera->tile_x;
        }
    }
    else if (dx < 0)
    {
        camera->offset_x -= camera->speed * -dx * elapsed_seconds;

        if (camera->offset_x <= -tile_width)
        {
            camera->offset_x = 0;
            --camera->tile_x;
        }
    }

    if (dy > 0)
    {
        camera->offset_y += camera->speed * dy * elapsed_seconds;

        if (camera->offset_y >= tile_height)
        {
            camera->offset_y = 0;
            ++camera->tile_y;
        }
    }
    else if (dy < 0)
    {
        camera->offset_y -= camera->speed * -dy * elapsed_seconds;

        if (camera->offset_y <= -tile_height)
        {
            camera->offset_y = 0;
            --camera->tile_y;
        }
    }
}

//  ---------------------------------------------------------------------------
void update_player_camera(struct Game* game)
{
    struct Actor* player_actor = game->world->player.actor;

    if (player_actor != NULL &&
        player_actor->map != NULL &&
        player_actor->tile != NULL)
    {
        update_camera(
            &game->world->camera,
            player_actor->tile->x,
            player_actor->tile->y,
            player_actor->map->tile_width,
            player_actor->map->tile_height,
            game->elapsed_seconds);
    }
}