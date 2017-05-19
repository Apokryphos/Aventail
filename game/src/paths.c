#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
char* CreateAssetPath(
    const char* basePath, 
    const char* assetPath, 
    const char* assetFilename,
    const char* extension)
{
    assert(basePath != NULL);
    assert(assetPath != NULL);
    assert(assetFilename != NULL);

    char *fullPath = malloc(strlen(basePath) + strlen(assetPath) + strlen(assetFilename) + strlen(extension) + 1);
    if (fullPath == NULL)
    {
		printf("Could not create asset file path.");
		exit(1);
    } 
    sprintf(fullPath, "%s%s%s%s", basePath, assetPath, assetFilename, extension);
    return fullPath;
}

//  ---------------------------------------------------------------------------
char* CreateMapPath(const char* basePath, const char* assetFilename)
{
    return CreateAssetPath(basePath, "assets/maps/", assetFilename, ".map");
}

//  ---------------------------------------------------------------------------
char* CreateSfxPath(const char* basePath, const char* assetFilename)
{
    return CreateAssetPath(basePath, "assets/sfx/", assetFilename, ".ogg");
}

//  ---------------------------------------------------------------------------
char* CreateTexturePath(const char* basePath, const char* assetFilename)
{
    return CreateAssetPath(basePath, "assets/gfx/", assetFilename, ".png");
}