#include "actor.h"
#include "actor_ai.h"
#include "actor_list.h"
#include "flip_flag.h"
#include "inventory.h"
#include "item.h"
#include "map.h"
#include "map_link.h"
#include "paths.h"
#include "string_util.h"
#include "tile.h"
#include "xml_util.h"
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <assert.h>
#include <string.h>

//  ---------------------------------------------------------------------------
static int is_collision_tile(const int tileset_id)
{
    //  TODO: Temp hack until tileset loading is complete
    switch (tileset_id)
    {
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 40:
        case 41:
        case 42:
        case 56:
        case 57:
        case 58:
        case 66:
            return 1;
    }

    return 0;
}

//  ---------------------------------------------------------------------------
static xmlNode* get_properties_node(xmlNode* parent_node)
{
    xmlNode* node = parent_node->xmlChildrenNode;
    while (node != NULL)
    {
        if (is_node(node, "properties"))
        {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
static xmlNode* get_property_node(
    xmlNode* properties_node,
    const char* property_name)
{
    if (properties_node != NULL)
    {
        xmlNode* node = properties_node->xmlChildrenNode;
        while (node != NULL)
        {
            if (is_node(node, "property"))
            {
                char* name = NULL;
                read_attribute(node, "name", &name);

                if (strcmp(name, property_name) == 0)
                {
                    free(name);
                    return node;
                }

                free(name);
            }
            node = node->next;
        }
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
static void read_bool_property(
    xmlNode* properties_node,
    char* property_name,
    int* value)
{
    xmlNode* property_node = get_property_node(properties_node, property_name);
    if (property_node != NULL)
    {
        read_bool_attribute(property_node, "value", value);
    }
}

//  ---------------------------------------------------------------------------
static void read_int_property(
    xmlNode* properties_node,
    char* property_name,
    int* value)
{
    xmlNode* property_node = get_property_node(properties_node, property_name);
    if (property_node != NULL)
    {
        read_int_attribute(property_node, "value", value);
    }
}

//  ---------------------------------------------------------------------------
static void read_property(
    xmlNode* properties_node,
    char* property_name,
    char** value)
{
    xmlNode* property_node = get_property_node(properties_node, property_name);
    if (property_node != NULL)
    {
        read_attribute(property_node, "value", value);
    }
}

//  ---------------------------------------------------------------------------
static void load_actor_items(struct Actor* actor, xmlNode* properties_node)
{
    size_t itemCount = 0;
    char** itemNames = NULL;
    char* itemNamesCsv = NULL;
    read_property(properties_node, "Items", &itemNamesCsv);
    itemCount = tokenize_string(itemNamesCsv, &itemNames);
    assert(itemCount < MAX_INVENTORY_ITEMS);
    for (int n = 0; n < itemCount; ++n)
    {
        struct Item* item = create_item(itemNames[n]);
        add_item_to_inventory(actor->inventory, item);
        free(itemNames[n]);
    }
    free(itemNames);
}

//  ---------------------------------------------------------------------------
void load_gid(unsigned int raw_gid, int* gid, int* flip_flags)
{
    const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
    const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
    const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

    const int flip_flags_mask = (
        FLIPPED_HORIZONTALLY_FLAG |
        FLIPPED_VERTICALLY_FLAG |
        FLIPPED_DIAGONALLY_FLAG);

    //  Remove flip flags and adjust so void tiles are -1
    (*gid) = (raw_gid & ~flip_flags_mask) - 1;

    //  Normal
    //  X&Y-Flip 180
    (*flip_flags) = 0;

    if (raw_gid & FLIPPED_DIAGONALLY_FLAG)
    {
        if ((raw_gid & FLIPPED_HORIZONTALLY_FLAG) &&
            (raw_gid & FLIPPED_VERTICALLY_FLAG))
        {
            //  X-Flip 90  HVD
            //  Y-Flip 270 HVD
            (*flip_flags) |= FLIP_FLAG_ROTATE_RIGHT;
            (*flip_flags) |= FLIP_FLAG_HORZ;
        }
        else if (raw_gid & FLIPPED_HORIZONTALLY_FLAG)
        {
            //            90 H D
            //  X&Y-Flip 270 H D
            (*flip_flags) |= FLIP_FLAG_ROTATE_RIGHT;
        }
        else if (raw_gid & FLIPPED_VERTICALLY_FLAG)
        {
            //          270  VD
            //  X&Y-Flip 90  VD
            (*flip_flags) |= FLIP_FLAG_ROTATE_LEFT;
        }
        else
        {
            //  X-Flip 270   D
            //   Y-Flip 90   D
            (*flip_flags) |= FLIP_FLAG_ROTATE_LEFT;
            (*flip_flags) |= FLIP_FLAG_HORZ;
        }
    }
    else
    {
        if ((raw_gid & FLIPPED_HORIZONTALLY_FLAG) &&
            (raw_gid & FLIPPED_VERTICALLY_FLAG))
        {
            //              180 HV
            //  X&Y-Flip Normal HV
            (*flip_flags) |= FLIP_FLAG_HORZ | FLIP_FLAG_VERT;
        }
        else if (raw_gid & FLIPPED_HORIZONTALLY_FLAG)
        {
            //  X-Flip Normal H
            //     Y-Flip 180 H
            (*flip_flags) |= FLIP_FLAG_HORZ;
        }
        else if (raw_gid & FLIPPED_VERTICALLY_FLAG)
        {
            //     X-Flip 180  V
            //  Y-Flip Normal  V
            (*flip_flags) |= FLIP_FLAG_VERT;
        }
    }
}

//  ---------------------------------------------------------------------------
void load_tmx(const xmlDoc* doc, struct Map** map, struct ActorList** actors)
{
    assert(doc != NULL);
    assert(*map == NULL);
    assert(*actors == NULL);

    xmlNode* root = xmlDocGetRootElement(doc);
    assert(root != NULL);
    assert(is_node(root, "map"));

    assert(xmlStrcmp(root->name, (const xmlChar*)"map") == 0);

    int map_width, map_height, tile_width, tile_height;
    read_int_attribute(root, "width", &map_width);
    read_int_attribute(root, "height", &map_height);
    read_int_attribute(root, "tilewidth", &tile_width);
    read_int_attribute(root, "tileheight", &tile_height);

    xmlNode* map_properties_node = get_properties_node(root);

    printf("Parsing TMX...\n");
    printf("Map Size: %d x %d\n", map_width, map_height);
    printf("Tile Size: %d x %d\n", tile_width, tile_height);

    *map = create_map(map_width, map_height, tile_width, tile_height);
    *actors = create_actor_list();

    read_bool_property(map_properties_node, "Sunlight", &(*map)->sunlight);

    xmlNode* node = root->xmlChildrenNode;
    while (node != NULL)
    {
        //  Tile layer
        if (is_node(node, "layer"))
        {
            xmlNode* data_node = node->xmlChildrenNode;
            while (data_node != NULL)
            {
                if (is_node(data_node, "data"))
                {
                    int tile_x = 0;
                    int tile_y = 0;

                    xmlNode* tile_node = data_node->xmlChildrenNode;
                    while (tile_node != NULL)
                    {
                        if (is_node(tile_node, "tile"))
                        {
                            struct Tile* tile = get_map_tile(*map, tile_x, tile_y);

                            unsigned int raw_gid;
                            read_unsigned_int_attribute(tile_node, "gid", &raw_gid);
                            load_gid(raw_gid, &tile->tileset_id, &tile->flip_flags);

                            tile->collision = is_collision_tile(tile->tileset_id);

                            ++tile_x;
                            if (tile_x > map_width - 1)
                            {
                                tile_x = 0;
                                ++tile_y;
                            }
                        }

                        tile_node = tile_node->next;
                    }
                }
                data_node = data_node->next;
            }
        }
        else if (is_node(node, "objectgroup"))
        {
            xmlNode* object_node = node->xmlChildrenNode;
            while (object_node != NULL)
            {
                if (is_node(object_node, "object"))
                {
                    int gid = 0;
                    int flip_flags = 0;
                    int tile_x = 0;
                    int tile_y = 0;
                    char* name = NULL;

                    read_attribute(object_node, "name", &name);
                    read_int_attribute(object_node, "x", &tile_x);
                    read_int_attribute(object_node, "y", &tile_y);

                    tile_x /= (*map)->tile_width;
                    tile_y /= (*map)->tile_height;

                    if (has_attribute(object_node, "gid"))
                    {
                        unsigned int raw_gid;
                        read_unsigned_int_attribute(object_node, "gid", &raw_gid);
                        load_gid(raw_gid, &gid, &flip_flags);

                        //  Adjust Y component of Tile objects
                        --tile_y;
                    }

                    xmlNode* properties_node = get_properties_node(object_node);

                    int cash = 0;
                    read_int_property(properties_node, "Cash", &cash);

                    char* type = NULL;
                    read_attribute(object_node, "type", &type);
                    if (strcmp(type, "Actor") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tile_x, tile_y, gid, flip_flags);
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "Villain") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tile_x, tile_y, gid, flip_flags);
                        actor->type = ACTOR_TYPE_VILLAIN;
                        actor->ai = create_actor_ai();
                        actor->cash = cash;
                        load_actor_items(actor, properties_node);
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "Container") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tile_x, tile_y, gid, flip_flags);
                        actor->type = ACTOR_TYPE_CONTAINER;
                        actor->cash = cash;
                        load_actor_items(actor, properties_node);
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "Door") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tile_x, tile_y, gid, flip_flags);
                        actor->type = ACTOR_TYPE_DOOR;
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "MapLink") == 0)
                    {
                        char* dest_map = NULL;
                        int dest_x, dest_y;

                        read_property(properties_node, "DestMap", &dest_map);
                        read_int_property(properties_node, "DestX", &dest_x);
                        read_int_property(properties_node, "DestY", &dest_y);

                        struct MapLink* link = create_map_link(dest_map, dest_x, dest_y);

                        struct Tile* tile = get_map_tile(*map, tile_x, tile_y);
                        tile->link = link;

                        free(dest_map);
                    }

                    if (name != NULL)
                    {
                        free(name);
                    }

                    if (type != NULL)
                    {
                        free(type);
                    }
                }
                object_node = object_node->next;
            }
        }
        node = node->next;
    }
}