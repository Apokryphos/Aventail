#include <unity.h>
#include <stdlib.h>
#include "map.h"

void test_create_map(void)
{
    struct Map* map = create_map(10, 15, 8, 12);
    TEST_ASSERT_EQUAL_INT(map->width, 10);
    TEST_ASSERT_EQUAL_INT(map->height, 15);
    TEST_ASSERT_EQUAL_INT(map->tile_width, 8);
    TEST_ASSERT_EQUAL_INT(map->tile_height, 12);
    destroy_map(&map);
}

void test_create_map_max_sizes()
{
    struct Map* map = create_map(MAX_MAP_SIZE, MAX_MAP_SIZE, 16, 16);
    TEST_ASSERT_EQUAL_INT(map->width, MAX_MAP_SIZE);
    TEST_ASSERT_EQUAL_INT(map->height, MAX_MAP_SIZE);
    destroy_map(&map);
}

void test_create_map_negative_sizes()
{
    struct Map* map = create_map(-1, 8, 16, 16);
    TEST_ASSERT(map == NULL);

    map = create_map(8, -1, 16, 16);
    TEST_ASSERT(map == NULL);
}

void test_create_map_sizes_exceed_max()
{
    struct Map* map = create_map(MAX_MAP_SIZE + 1, MAX_MAP_SIZE, 16, 16);
    TEST_ASSERT(map == NULL);

    map = create_map(MAX_MAP_SIZE, MAX_MAP_SIZE + 1, 16, 16);
    TEST_ASSERT(map == NULL);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_create_map);
    RUN_TEST(test_create_map_max_sizes);
    RUN_TEST(test_create_map_negative_sizes);
    RUN_TEST(test_create_map_sizes_exceed_max);
    return UNITY_END();
}