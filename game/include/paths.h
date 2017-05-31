#ifndef PATHS_HEADER_DEFINED
#define PATHS_HEADER_DEFINED

char* create_map_file_path(const char* asset_filename);
char* create_sfx_file_path(const char* asset_filename);
char* create_texture_file_path(const char* asset_filename);
void paths_init();
void paths_shutdown();

#endif