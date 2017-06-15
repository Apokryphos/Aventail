#include "actor.h"
#include "actor_list.h"
#include "flip_flag.h"
#include "game.h"
#include "map.h"
#include "light.h"
#include "panel.h"
#include "paths.h"
#include "tile.h"
#include "tileset.h"
#include "world.h"
#include "zone.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <math.h>
#include <string.h>

//  SDL_RenderSetScale scale amount
static const int RENDER_SCALE = 2;

static SDL_Renderer* renderer = NULL;

static struct Tileset font_tileset;
static struct Tileset gui_tileset;
static struct Tileset map_tileset;

//  ---------------------------------------------------------------------------
void get_viewport(SDL_Rect* rect)
{
     SDL_RenderGetViewport(renderer, rect);
}

//  ---------------------------------------------------------------------------
void get_viewport_center(int* x, int *y)
{
    SDL_Rect viewport;
    SDL_RenderGetViewport(renderer, &viewport);
    *x = (viewport.w / 2);
    *y = (viewport.h / 2);
}

//  ---------------------------------------------------------------------------
void load_texture(SDL_Texture **texture, const char* asset_name)
{
    char *full_path = create_texture_file_path(asset_name);
    *texture = IMG_LoadTexture(renderer, full_path);
    free(full_path);
}

//  ---------------------------------------------------------------------------
void measure_text(const char* text, int* width, int* height)
{
    assert(text != NULL);

    *width = strlen(text) * font_tileset.tile_width;
    *height = font_tileset.tile_height;
}

//  ---------------------------------------------------------------------------
void draw_screen_fade(float progress)
{
    if (progress < 0)
    {
        progress = 0;
    }
    else if (progress > 1)
    {
        progress = 1;
    }

    Uint8 alpha = (int)(255 * progress);

    SDL_Rect dest_rect;
    SDL_RenderGetViewport(renderer, &dest_rect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(renderer, &dest_rect);

    if (alpha != 255)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

//  ---------------------------------------------------------------------------
void draw_text(
    const char* text,
    const int x,
    const int y)
{
    assert(renderer != NULL);
    assert(text != NULL);
    assert(font_tileset.texture != NULL);

    SDL_Rect dest_rect =
    {
        .x = x,
        .y = y,
        .w = font_tileset.tile_width,
        .h = font_tileset.tile_height
    };
    SDL_Rect src_rect;

    int text_len = strlen(text);
    for (int t = 0; t < text_len; ++t)
    {
        int c = (int)text[t];
        if (c > 32 && c < 127)
        {
            get_tileset_rect(&font_tileset, c - 32, &src_rect);
            SDL_RenderCopy(renderer, font_tileset.texture, &src_rect, &dest_rect);
        }

        dest_rect.x += dest_rect.w;
    }
}

//  ---------------------------------------------------------------------------
void draw_text_centered(
    const char* text,
    const int x,
    const int y)
{
    int text_width, text_height;
    measure_text(text, &text_width, &text_height);
    draw_text(text, x - text_width / 2, y - text_height / 2);
}

//  ---------------------------------------------------------------------------
void draw_map(struct World* world)
{
    struct Map* map = world->zone->map;
    struct ActorList* actors = world->zone->actors;
    struct Actor* player_actor = world->player.actor;

    assert(renderer != NULL);
    assert(actors != NULL);
    assert(map != NULL);
    assert(map_tileset.texture != NULL);
    assert(player_actor != NULL);

    SDL_Rect src_rect;

    SDL_Rect dest_rect;
    dest_rect.w = map->tile_width;
    dest_rect.h = map->tile_height;

    for (int y = 0; y < map->height; ++y)
    {
        for (int x = 0; x < map->width; ++x)
        {
            int tileset_id = map->tiles[y * map->width + x].tileset_id;

            if (tileset_id < 0)
            {
                continue;
            }

            int flip_flags = map->tiles[y * map->width + x].flip_flags;

            SDL_RendererFlip sdl_flip_flags =
                ((flip_flags & FLIP_FLAG_HORZ) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) |
                ((flip_flags & FLIP_FLAG_VERT) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);

            double rotation =
                (flip_flags & FLIP_FLAG_ROTATE_RIGHT) ? 90 :
                (flip_flags & FLIP_FLAG_ROTATE_LEFT) ? 270 :
                0;

            get_tileset_rect(&map_tileset, tileset_id, &src_rect);

            dest_rect.x = x * dest_rect.w;
            dest_rect.y = y * dest_rect.h;

            SDL_RenderCopyEx(
                renderer,
                map_tileset.texture,
                &src_rect,
                &dest_rect,
                rotation,
                NULL,
                sdl_flip_flags);
        }
    }

    sort_actor_list(actors);

    struct ActorListNode* actor_node = actors->front;
    while (actor_node != NULL)
    {
        struct Actor* actor = actor_node->actor;
        if (actor->tile != NULL)
        {
            int flip_flags = actor->flip_flags;

            SDL_RendererFlip sdl_flip_flags =
                ((flip_flags & FLIP_FLAG_HORZ) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE) |
                ((flip_flags & FLIP_FLAG_VERT) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);

            double rotation =
                (flip_flags & FLIP_FLAG_ROTATE_RIGHT) ? 90 :
                (flip_flags & FLIP_FLAG_ROTATE_LEFT) ? 270 :
                actor->rotation;

            get_tileset_rect(&map_tileset, actor->tileset_id, &src_rect);

            dest_rect.x = actor->tile->x * dest_rect.w;
            dest_rect.y = actor->tile->y * dest_rect.h;

            SDL_RenderCopyEx(
                renderer,
                map_tileset.texture,
                &src_rect,
                &dest_rect,
                rotation,
                NULL,
                sdl_flip_flags);
        }

        actor_node = actor_node->next;
    }

    //  Draw lighting
    if (map->sunlight == 0)
    {
        update_lighting(map, player_actor, actors);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int y = 0; y < map->height; ++y)
        {
            for (int x = 0; x < map->width; ++x)
            {
                dest_rect.x = x * dest_rect.w;
                dest_rect.y = y * dest_rect.h;

                int light = map->tiles[y * map->width + x].light;

                if (light >= 255)
                {
                    continue;
                }
                else
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255 - light);
                    SDL_RenderFillRect(renderer, &dest_rect);
                }
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
}

//  ---------------------------------------------------------------------------
void draw_tileset_tile(
    int tileset_id,
    const int x,
    const int y,
    enum FlipFlag flip)
{
    SDL_Rect src_rect;

    SDL_Rect dest_rect;
    dest_rect.w = map_tileset.tile_width;
    dest_rect.h = map_tileset.tile_height;

    get_tileset_rect(&map_tileset, tileset_id, &src_rect);

    dest_rect.x = x;
    dest_rect.y = y;

    SDL_RendererFlip flip_flags =
        (flip & FLIP_FLAG_HORZ ? SDL_FLIP_HORIZONTAL : 0) |
        (flip & FLIP_FLAG_VERT ? SDL_FLIP_VERTICAL : 0);

    SDL_RenderCopyEx(
        renderer,
        map_tileset.texture,
        &src_rect,
        &dest_rect,
        0,
        NULL,
        flip_flags);
}

//  ---------------------------------------------------------------------------
void draw_gui_tileset_tile(
    const int tileset_id,
    const int x,
    const int y,
    enum FlipFlag flip)
{
    SDL_Rect src_rect;

    SDL_Rect dest_rect;
    dest_rect.w = gui_tileset.tile_width;
    dest_rect.h = gui_tileset.tile_height;

    get_tileset_rect(&gui_tileset, tileset_id, &src_rect);

    dest_rect.x = x;
    dest_rect.y = y;

    SDL_RendererFlip flipFlags =
        (flip & FLIP_FLAG_HORZ ? SDL_FLIP_HORIZONTAL : 0) |
        (flip & FLIP_FLAG_VERT ? SDL_FLIP_VERTICAL : 0);

    SDL_RenderCopyEx(
        renderer,
        gui_tileset.texture,
        &src_rect,
        &dest_rect,
        0,
        NULL,
        flipFlags);
}

//  ---------------------------------------------------------------------------
static void draw_panel_text(const struct Panel* panel)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(panel->text != NULL);

    int text_width, text_height, text_x, text_y;
    measure_text(panel->text, &text_width, &text_height);
    if (panel->text_align == PANEL_TEXT_ALIGN_LEFT)
    {
        text_x = panel->x;
        text_y = panel->y;
    }
    else
    {
        text_x = panel->x + (panel->width / 2) - (text_width / 2);
        text_y = panel->y + (panel->height / 2) - (text_height / 2);
    }
    draw_text(panel->text, text_x, text_y);
}

//  ---------------------------------------------------------------------------
static void draw_panel_title(const struct Panel* panel, const char* text)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(text != NULL);

    int text_width, text_height;
    measure_text(text, &text_width, &text_height);
    int text_x = panel->x + (panel->width / 2) - (text_width / 2);
    int text_y = panel->y - text_height;
    draw_text(text, text_x, text_y);
}

//  ---------------------------------------------------------------------------
void draw_panel_border(const struct Panel* panel)
{
    if (panel->border_style == PANEL_BORDER_STYLE_NONE)
    {
        return;
    }

    SDL_Rect src_rect;

    SDL_Rect dest_rect;
    dest_rect.x = panel->x;
    dest_rect.y = panel->y;
    dest_rect.w = gui_tileset.tile_width;
    dest_rect.h = gui_tileset.tile_height;

    int corner_tileset_id, side_tileset_id;
    get_panel_border_tileset_ids(
        panel->border_style,
        &corner_tileset_id,
        &side_tileset_id);

    get_tileset_rect(&gui_tileset, side_tileset_id, &src_rect);
    int start_x = panel->x + dest_rect.w;
    int end_x = panel->x + panel->width - dest_rect.w;
    for (int x = start_x; x < end_x; x += dest_rect.w)
    {
        dest_rect.x = x;

        dest_rect.y = panel->y;
        SDL_RenderCopyEx(
            renderer,
            gui_tileset.texture,
            &src_rect,
            &dest_rect,
            90,
            NULL,
            SDL_FLIP_NONE);

        dest_rect.y = panel->y + panel->height - gui_tileset.tile_height;
        SDL_RenderCopyEx(
            renderer,
            gui_tileset.texture,
            &src_rect,
            &dest_rect,
            270,
            NULL,
            SDL_FLIP_NONE);
    }

    int start_y = panel->y + dest_rect.h;
    int end_y = panel->y + panel->height - dest_rect.h;
    for (int y = start_y; y < end_y; y += dest_rect.h)
    {
        dest_rect.y = y;

        dest_rect.x = panel->x;
        SDL_RenderCopy(renderer, gui_tileset.texture, &src_rect, &dest_rect);

        dest_rect.x = panel->x + panel->width - gui_tileset.tile_width;
        SDL_RenderCopyEx(
            renderer,
            gui_tileset.texture,
            &src_rect,
            &dest_rect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL);
    }

    get_tileset_rect(&gui_tileset, corner_tileset_id, &src_rect);

    //  Upper-left corner
    dest_rect.x = panel->x;
    dest_rect.y = panel->y;
    SDL_RenderCopy(renderer, gui_tileset.texture, &src_rect, &dest_rect);

    //  Lower-left corner
    dest_rect.x = panel->x;
    dest_rect.y = panel->y + panel->height - gui_tileset.tile_height;
    SDL_RenderCopyEx(
            renderer,
            gui_tileset.texture,
            &src_rect,
            &dest_rect,
            0,
            NULL,
            SDL_FLIP_VERTICAL);

    //  Lower-right corner
    dest_rect.x = panel->x + panel->width - gui_tileset.tile_width;
    dest_rect.y = panel->y + panel->height - gui_tileset.tile_height;
    SDL_RenderCopyEx(
            renderer,
            gui_tileset.texture,
            &src_rect,
            &dest_rect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);

    //  Upper-right corner
    dest_rect.x = panel->x + panel->width - gui_tileset.tile_width;
    dest_rect.y = panel->y;
    SDL_RenderCopyEx(
            renderer,
            gui_tileset.texture,
            &src_rect,
            &dest_rect,
            0,
            NULL,
            SDL_FLIP_HORIZONTAL);
}

//  ---------------------------------------------------------------------------
void draw_panel(const struct Panel* panel)
{
    assert(renderer != NULL);
    assert(panel != NULL);
    assert(map_tileset.texture != NULL);

    if (panel->visible == 0)
    {
        return;
    }

    if (panel->show_title && panel->title != NULL)
    {
        draw_panel_title(panel, panel->title);
    }

    if (panel->background)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_Rect panelRect = { panel->x, panel->y, panel->width, panel->height };
        SDL_SetRenderDrawColor(renderer, 42, 5, 3, panel->alpha);
        SDL_RenderFillRect(renderer, &panelRect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    if (panel->alpha < 255)
    {
        SDL_SetTextureBlendMode(gui_tileset.texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(gui_tileset.texture, panel->alpha);
    }

    draw_panel_border(panel);

    if (panel->icon.tileset_id > -1)
    {
        if (panel->icon.style == PANEL_ICON_STYLE_NORMAL)
        {
            draw_tileset_tile(
                panel->icon.tileset_id,
                panel->x + (panel->width / 2) - (map_tileset.tile_width / 2),
                panel->y + (panel->height / 2) - (map_tileset.tile_height / 2),
                panel->icon.flip);
        }
        else if (panel->icon.style == PANEL_ICON_STYLE_SMALL)
        {
            draw_gui_tileset_tile(
                panel->icon.tileset_id,
                panel->x + (panel->width / 2) - (gui_tileset.tile_width / 2),
                panel->y + (panel->height / 2) - (gui_tileset.tile_height / 2),
                panel->icon.flip);
        }
    }

    if (panel->text != NULL)
    {
        draw_panel_text(panel);
    }

    if (panel->alpha < 255)
    {
        SDL_SetTextureBlendMode(gui_tileset.texture, SDL_BLENDMODE_NONE);
        SDL_SetTextureAlphaMod(gui_tileset.texture, 255);
    }
}

//  ---------------------------------------------------------------------------
void draw_alpha_text(
    const char* text,
    const int x,
    const int y,
    const int alpha)
{
    SDL_SetTextureAlphaMod(font_tileset.texture, alpha);
    draw_text(text, x, y);
    SDL_SetTextureAlphaMod(font_tileset.texture, 255);
}

//  ---------------------------------------------------------------------------
void get_tile_rect(
    const struct Map* map,
    const struct Tile* tile,
    SDL_Rect* rect,
    const int scaled)
{
    assert(map != NULL);
    assert(tile != NULL);
    assert(rect != NULL);

    int scale = scaled ? RENDER_SCALE : 1;

    (*rect).x = tile->x * map->tile_width * scale;
    (*rect).y = tile->y * map->tile_height * scale;
    (*rect).w = map->tile_width * scale;
    (*rect).h = map->tile_height * scale;
}

//  ---------------------------------------------------------------------------
void render_clear()
{
    assert(renderer != NULL);
    SDL_RenderClear(renderer);
}

//  ---------------------------------------------------------------------------
int render_init(SDL_Window* window)
{
    assert(renderer == NULL);
    assert(font_tileset.texture == NULL);
    assert(gui_tileset.texture == NULL);
    assert(map_tileset.texture == NULL);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
		fprintf(stderr, "SDL failed to create window: %sn", SDL_GetError());
		exit(EXIT_FAILURE);
    }

    //  Clear window
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_RenderSetScale(renderer, RENDER_SCALE, RENDER_SCALE);

    load_tileset(&font_tileset, "font");
    if (font_tileset.texture == NULL)
    {
        fprintf(stderr, "Failed to load font tileset texture.\n");
        return 1;
    }

    font_tileset.tile_width = 5;
    font_tileset.tile_height = 12;

    load_tileset(&gui_tileset, "tileset");
    if (gui_tileset.texture == NULL)
    {
        fprintf(stderr, "Failed to load GUI tileset texture.\n");
        return 1;
    }

    gui_tileset.tile_width = 8;
    gui_tileset.tile_height = 8;

    load_tileset(&map_tileset, "tileset");
    if (map_tileset.texture == NULL)
    {
        fprintf(stderr, "Failed to load map tileset texture.\n");
        return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
void render_shutdown()
{
    SDL_DestroyTexture(font_tileset.texture);
    SDL_DestroyTexture(gui_tileset.texture);
    SDL_DestroyTexture(map_tileset.texture);

    if (renderer != NULL)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
}

//  ---------------------------------------------------------------------------
void render_swap()
{
    SDL_RenderPresent(renderer);
}