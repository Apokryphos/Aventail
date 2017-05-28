#include "actor.h"
#include "actor_list.h"
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
int IsCollision(const int tilesetId)
{
    //  TODO: Temp hack until tileset loading is complete
    switch (tilesetId)
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
xmlNode* GetPropertiesNode(xmlNode* parentNode)
{
    xmlNode* node = parentNode->xmlChildrenNode;
    while (node != NULL)
    {
        if (IsNode(node, "properties"))
        {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

//  ---------------------------------------------------------------------------
xmlNode* GetPropertyNode(xmlNode* propertiesNode, const char* propertyName)
{
    if (propertiesNode != NULL)
    {
        xmlNode* node = propertiesNode->xmlChildrenNode;
        while (node != NULL)
        {
            if (IsNode(node, "property"))
            {
                char* name = NULL;
                ReadAttribute(node, "name", &name);

                if (strcmp(name, propertyName) == 0)
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
void ReadIntProperty(xmlNode* propertiesNode, char* propertyName, int* value)
{
    xmlNode* propertyNode = GetPropertyNode(propertiesNode, propertyName);
    if (propertyNode != NULL)
    {
        ReadIntAttribute(propertyNode, "value", value);
    }
}

//  ---------------------------------------------------------------------------
void ReadProperty(xmlNode* propertiesNode, char* propertyName, char** value)
{
    xmlNode* propertyNode = GetPropertyNode(propertiesNode, propertyName);
    if (propertyNode != NULL)
    {
        ReadAttribute(propertyNode, "value", value);
    }
}

//  ---------------------------------------------------------------------------
void LoadActorItems(struct Actor* actor, xmlNode* propertiesNode)
{
    size_t itemCount = 0;
    char** itemNames = NULL;
    char* itemNamesCsv = NULL;
    ReadProperty(propertiesNode, "Items", &itemNamesCsv);
    itemCount = Tokenize(itemNamesCsv, &itemNames);
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
void LoadTmx(const xmlDoc* doc, struct Map** map, struct ActorList** actors)
{
    assert(doc != NULL);
    assert(*map == NULL);
    assert(*actors == NULL);

    xmlNode* root = xmlDocGetRootElement(doc);
    assert(root != NULL);
    assert(IsNode(root, "map"));

    assert(xmlStrcmp(root->name, (const xmlChar*)"map") == 0);

    int mapWidth, mapHeight, tileWidth, tileHeight;
    ReadIntAttribute(root, "width", &mapWidth);
    ReadIntAttribute(root, "height", &mapHeight);
    ReadIntAttribute(root, "tilewidth", &tileWidth);
    ReadIntAttribute(root, "tileheight", &tileHeight);

    printf("Parsing TMX...\n");
    printf("Map Size: %d x %d\n", mapWidth, mapHeight);
    printf("Tile Size: %d x %d\n", tileWidth, tileHeight);

    *map = create_map(mapWidth, mapHeight, tileWidth, tileHeight);
    *actors = create_actor_list();

    xmlNode* node = root->xmlChildrenNode;
    while (node != NULL)
    {
        //  Tile layer
        if (IsNode(node, "layer"))
        {
            xmlNode* dataNode = node->xmlChildrenNode;
            while (dataNode != NULL)
            {
                if (IsNode(dataNode, "data"))
                {
                    int tileX = 0;
                    int tileY = 0;

                    xmlNode* tileNode = dataNode->xmlChildrenNode;
                    while (tileNode != NULL)
                    {
                        if (IsNode(tileNode, "tile"))
                        {
                            int gid;
                            ReadIntAttribute(tileNode, "gid", &gid);

                            struct Tile* tile = get_map_tile(*map, tileX, tileY);
                            tile->TilesetId = gid - 1;

                            tile->Collision = IsCollision(tile->TilesetId);

                            ++tileX;
                            if (tileX > mapWidth - 1)
                            {
                                tileX = 0;
                                ++tileY;
                            }
                        }

                        tileNode = tileNode->next;
                    }
                }
                dataNode = dataNode->next;
            }
        }
        else if (IsNode(node, "objectgroup"))
        {
            xmlNode* objectNode = node->xmlChildrenNode;
            while (objectNode != NULL)
            {
                if (IsNode(objectNode, "object"))
                {
                    int gid = 0;
                    int tileX = 0;
                    int tileY = 0;
                    char* name = NULL;

                    ReadAttribute(objectNode, "name", &name);
                    ReadIntAttribute(objectNode, "x", &tileX);
                    ReadIntAttribute(objectNode, "y", &tileY);

                    tileX /= (*map)->tile_width;
                    tileY /= (*map)->tile_height;

                    if (HasAttribute(objectNode, "gid"))
                    {
                        ReadIntAttribute(objectNode, "gid", &gid);

                        //  Adjust GID
                        --gid;

                        //  Adjust Y component of Tile objects
                        --tileY;
                    }

                    xmlNode* propertiesNode = GetPropertiesNode(objectNode);

                    int cash = 0;
                    ReadIntProperty(propertiesNode, "Cash", &cash);

                    char* type = NULL;
                    ReadAttribute(objectNode, "type", &type);
                    if (strcmp(type, "Actor") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tileX, tileY, gid);
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "Villain") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tileX, tileY, gid);
                        actor->type = ACTOR_TYPE_VILLAIN;
                        actor->cash = cash;
                        LoadActorItems(actor, propertiesNode);
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "Container") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tileX, tileY, gid);
                        actor->type = ACTOR_TYPE_CONTAINER;
                        actor->cash = cash;
                        LoadActorItems(actor, propertiesNode);
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "Door") == 0)
                    {
                        struct Actor* actor = create_actor(*map, name, tileX, tileY, gid);
                        actor->type = ACTOR_TYPE_DOOR;
                        add_actor_to_actor_list_back(*actors, actor);
                    }
                    else if (strcmp(type, "MapLink") == 0)
                    {
                        char* destMap = NULL;
                        int destX, destY;

                        ReadProperty(propertiesNode, "DestMap", &destMap);
                        ReadIntProperty(propertiesNode, "DestX", &destX);
                        ReadIntProperty(propertiesNode, "DestY", &destY);

                        struct MapLink* link = create_map_link(destMap, destX, destY);

                        struct Tile* tile = get_map_tile(*map, tileX, tileY);
                        tile->Link = link;

                        free(destMap);
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
                objectNode = objectNode->next;
            }
        }
        node = node->next;
    }
}