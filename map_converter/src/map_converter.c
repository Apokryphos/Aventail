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

void ConvertTmxToMap(const char* tmxFilename, const char* mapFilename);

//  ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    init_args(argc, argv);

    if (get_arg_count() == 2)
    {
        const char* tmxFilename = get_arg_by_index(1);
        const char* mapFilename = get_arg_by_index(2);

        ConvertTmxToMap(tmxFilename, mapFilename);
    }
    else if (get_arg_count() == 1)
    {
        const char* tmxFilename = get_arg_by_index(1);
        size_t len = strlen(tmxFilename);

        char* mapFilename = NULL;

        if (len >= 4)
        {
            const char* extension = tmxFilename + len - 4;
            if (strcasecmp(extension, ".tmx") == 0)
            {
                //  Replace ".tmx" with ".map"
                mapFilename = malloc(sizeof(char) * len + 1);
                strncpy(mapFilename, tmxFilename, len + 4);
                strncpy(mapFilename + len - 4, ".map", 4);
            }
        }

        if (mapFilename == NULL)
        {
            //  Add ".map" to end of filename retaining original extension
            mapFilename = malloc(sizeof(char) * len + 5);
            snprintf(mapFilename, len + 5, "%s.map", tmxFilename);
            mapFilename[len + 4] = '\0';
        }

        ConvertTmxToMap(tmxFilename, mapFilename);

        free(mapFilename);
    }
    else
    {
        printf("Usage: mc.exe TMX_FILE\n");
        printf("  or:  mc.exe TMX_FILE [MAP_FILE]\n");
        printf("Converts Tiled TMX file to MAP file.\n");
    }
}

//  ---------------------------------------------------------------------------
void ConvertTmxToMap(const char* tmxFilename, const char* mapFilename)
{
    xmlDoc* doc = xmlReadFile(tmxFilename, NULL, 0);
    if (doc == NULL)
    {
        printf("Error: Could not parse TMX file %s.\n", tmxFilename);
        return;
    }

    struct Map* map = NULL;
    struct ActorList* actors = NULL;

    LoadTmx(doc, &map, &actors);

    FILE *file;
    file = fopen(mapFilename,"wb");
    SaveMapToFile(file, map);
    SaveActorsToFile(file, actors);
    fclose(file);

    printf("Converted TMX file %s to %s.\n", tmxFilename, mapFilename);

    xmlFreeDoc(doc);
    xmlCleanupParser();
}