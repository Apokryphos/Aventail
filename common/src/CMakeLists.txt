set(COMMON_SOURCE
    actor_ai.c
    actor_list.c
    actor.c
    args.c
    direction.c
    gear.c
    inventory.c
    item_type.c
    item.c
    map_file.c
    map_link.c
    map_save_util.c
    map.c
    stats.c
    string_util.c
)

include_directories(${Aventail_SOURCE_DIR}/common/include)
add_library(common ${COMMON_SOURCE})