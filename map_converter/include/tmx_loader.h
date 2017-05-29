#ifndef TMX_LOADER_HEADER_INCLUDED
#define TMX_LOADER_HEADER_INCLUDED

struct ActorList;
struct Map;
typedef struct _xmlDoc xmlDoc;

void load_tmx(const xmlDoc* doc, struct Map** map, struct ActorList** actors);

#endif