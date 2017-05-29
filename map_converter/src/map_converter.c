#include "args.h"
#include "map.h"
#include "map_save_util.h"
#include "tmx_loader.h"
#include "xml_util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

void convert_tmx_to_map(const char* tmx_filename, const char* map_filename);

//  ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    init_args(argc, argv);

    if (get_arg_count() == 2)
    {
        const char* tmx_filename = get_arg_by_index(1);
        const char* map_filename = get_arg_by_index(2);

        convert_tmx_to_map(tmx_filename, map_filename);
    }
    else if (get_arg_count() == 1)
    {
        const char* tmx_filename = get_arg_by_index(1);
        size_t len = strlen(tmx_filename);

        char* map_filename = NULL;

        if (len >= 4)
        {
            const char* extension = tmx_filename + len - 4;
            if (strcasecmp(extension, ".tmx") == 0)
            {
                //  Replace ".tmx" with ".map"
                map_filename = malloc(sizeof(char) * len + 1);
                strncpy(map_filename, tmx_filename, len + 4);
                strncpy(map_filename + len - 4, ".map", 4);
            }
        }

        if (map_filename == NULL)
        {
            //  Add ".map" to end of filename retaining original extension
            map_filename = malloc(sizeof(char) * len + 5);
            snprintf(map_filename, len + 5, "%s.map", tmx_filename);
            map_filename[len + 4] = '\0';
        }

        convert_tmx_to_map(tmx_filename, map_filename);

        free(map_filename);
    }
    else
    {
        printf("Usage: mc.exe TMX_FILE\n");
        printf("  or:  mc.exe TMX_FILE [MAP_FILE]\n");
        printf("Converts Tiled TMX file to MAP file.\n");
    }
}

//  ---------------------------------------------------------------------------
void convert_tmx_to_map(const char* tmx_filename, const char* map_filename)
{
    xmlDoc* doc = xmlReadFile(tmx_filename, NULL, 0);
    if (doc == NULL)
    {
        printf("Error: Could not parse TMX file %s.\n", tmx_filename);
        return;
    }

    struct Map* map = NULL;
    struct ActorList* actors = NULL;

    load_tmx(doc, &map, &actors);

    FILE *file;
    file = fopen(map_filename,"wb");
    save_map_to_file(file, map);
    save_actors_to_file(file, actors);
    fclose(file);

    printf("Converted TMX file %s to %s.\n", tmx_filename, map_filename);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}