#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* base_path = NULL;

//  ---------------------------------------------------------------------------
static char* create_asset_file_path(
    const char* asset_path,
    const char* asset_filename,
    const char* extension)
{
    assert(base_path != NULL);
    assert(asset_path != NULL);
    assert(asset_filename != NULL);

    char *full_path = malloc(
        strlen(base_path) +
        strlen(asset_path) +
        strlen(asset_filename) +
        strlen(extension) + 1);

    if (full_path == NULL)
    {
		printf("Could not create asset file path.");
		exit(1);
    }

    sprintf(full_path, "%s%s%s%s", base_path, asset_path, asset_filename, extension);

    return full_path;
}

//  ---------------------------------------------------------------------------
char* create_map_file_path(const char* asset_filename)
{
    return create_asset_file_path("assets/maps/", asset_filename, ".map");
}

//  ---------------------------------------------------------------------------
char* create_sfx_file_path(const char* asset_filename)
{
    return create_asset_file_path("assets/sfx/", asset_filename, ".ogg");
}

//  ---------------------------------------------------------------------------
char* create_texture_file_path(const char* asset_filename)
{
    return create_asset_file_path("assets/gfx/", asset_filename, ".png");
}

//  ---------------------------------------------------------------------------
void paths_init()
{
    assert(base_path == NULL);

    base_path = SDL_GetBasePath();
    if (base_path == NULL)
    {
        base_path = SDL_strdup("./");
    }
}

//  ---------------------------------------------------------------------------
void paths_shutdown()
{
    if (base_path != NULL)
    {
        SDL_free(base_path);
        base_path = NULL;
    }
}