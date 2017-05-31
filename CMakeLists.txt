project("Aventail" C)
cmake_minimum_required(VERSION 3.0)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(LIBXML2 REQUIRED libxml-2.0 libxml2 libxml>=2)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2_IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2_MIXER REQUIRED SDL2_mixer>=2.0.0)

add_subdirectory(common/src)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_subdirectory(game/src)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/tools)
add_subdirectory(map_converter/src)

add_subdirectory(Unity)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin_tests)
add_subdirectory(test/src)