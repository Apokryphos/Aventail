set(MAP_CONVERTER_SOURCE
    map_converter.c
    tmx_loader.c
    xml_util.c
)

include_directories(${Aventail_SOURCE_DIR}/common/include)
include_directories(${Aventail_SOURCE_DIR}/map_converter/include)
include_directories(${LIBXML2_INCLUDE_DIRS})

add_executable(mc ${MAP_CONVERTER_SOURCE})
target_link_libraries(mc common ${LIBXML2_LIBRARIES})