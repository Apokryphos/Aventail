#ifndef CAMERA_HEADER_INCLUDED
#define CAMERA_HEADER_INCLUDED

struct Game;

struct Camera
{
    int tile_x;
    int tile_y;
    float offset_x;
    float offset_y;
    float speed;
};

void set_camera_position(
    struct Camera* camera,
    const int tile_x,
    const int tile_y);

void update_camera(
    struct Camera* camera,
    const int target_tile_x,
    const int target_tile_y,
    const int tile_width,
    const int tile_height,
    const float elapsed_seconds);

void update_player_camera(struct Game* game);

#endif