set(GAME_SOURCE
    actor_defs.c
    actor_funcs.c
    audio.c
    blink_state_data.c
    game_state_game_over.c
    game_state_gear.c
    game_state_inventory.c
    game_state_level.c
    game_state_status.c
    game_state_transition.c
    game_state.c
    game.c
    gui_gauge_string.c
    gui_int_string.c
    gui_screen.c
    gui_string.c
    gui.c
    input.c
    inventory_widget.c
    item_defs.c
    item_slot_widget.c
    main.c
    map_load_util.c
    panel.c
    path_finder.c
    paths.c
    render.c
    status_widget.c
    tileset.c
    world.c
)

include_directories(${Aventail_SOURCE_DIR}/game/include)
include_directories(${Aventail_SOURCE_DIR}/common/include)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
add_executable(game ${GAME_SOURCE})
target_link_libraries(game common ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})