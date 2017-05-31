set(TEST_SOURCE
    map_tests.c
)

include_directories(${Aventail_SOURCE_DIR}/common/include)
add_executable(map_tests ${TEST_SOURCE})
target_link_libraries(map_tests common unity)
add_test(map_test map_tests)