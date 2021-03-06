CC = gcc
LINKER := gcc
CFLAGS = -Wall -g -pedantic -D_GNU_SOURCE
OBJ_DIR = build/
MKDIR = @mkdir -p $(@D)

MAP_CONVERTER_OBJ_DIR = build/map_converter/

LIBXML2_INCLUDE = -I/usr/include/libxml2

COMMON_DIR = common/
COMMON_INCLUDE = -I$(COMMON_DIR)include
COMMON_OBJ_DIR = $(OBJ_DIR)$(COMMON_DIR)
COMMON_SRC_DIR = $(COMMON_DIR)src/
COMMON_SRC_FILES = $(wildcard $(COMMON_SRC_DIR)*.c)
COMMON_OBJ_FILES = $(patsubst $(COMMON_SRC_DIR)%.c,$(COMMON_OBJ_DIR)%.o,$(COMMON_SRC_FILES))
COMMON_DEP_FILES = $(patsubst $(COMMON_SRC_DIR)%.c,$(COMMON_OBJ_DIR)%.d,$(COMMON_SRC_FILES))

GAME_BIN_DIR = bin/
GAME_EXE = game.exe
GAME_DIR = game/
GAME_INCLUDE = -I$(GAME_DIR)include $(COMMON_INCLUDE)
GAME_LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lm
GAME_OBJ_DIR = $(OBJ_DIR)$(GAME_DIR)
GAME_SRC_DIR = $(GAME_DIR)src/
GAME_SRC_FILES = $(wildcard $(GAME_SRC_DIR)*.c)
GAME_OBJ_FILES = $(patsubst $(GAME_SRC_DIR)%.c,$(GAME_OBJ_DIR)%.o,$(GAME_SRC_FILES))
GAME_DEP_FILES = $(patsubst $(GAME_SRC_DIR)%.c,$(GAME_OBJ_DIR)%.d,$(GAME_SRC_FILES))

MAP_CONVERTER_BIN_DIR = tools/
MAP_CONVERTER_EXE = mc.exe
MAP_CONVERTER_DIR = map_converter/
MAP_CONVERTER_INCLUDE = -I$(MAP_CONVERTER_DIR)include $(COMMON_INCLUDE) $(LIBXML2_INCLUDE)
MAP_CONVERTER_LIBS = -lxml2
MAP_CONVERTER_OBJ_DIR = $(OBJ_DIR)$(MAP_CONVERTER_DIR)
MAP_CONVERTER_SRC_DIR = $(MAP_CONVERTER_DIR)src/
MAP_CONVERTER_SRC_FILES = $(wildcard $(MAP_CONVERTER_SRC_DIR)*.c)
MAP_CONVERTER_OBJ_FILES = $(patsubst $(MAP_CONVERTER_SRC_DIR)%.c,$(MAP_CONVERTER_OBJ_DIR)%.o,$(MAP_CONVERTER_SRC_FILES))
MAP_CONVERTER_DEP_FILES = $(patsubst $(MAP_CONVERTER_SRC_DIR)%.c,$(MAP_CONVERTER_OBJ_DIR)%.d,$(MAP_CONVERTER_SRC_FILES))

TEST_BIN_DIR = test_bin/
TEST_EXE = test.exe
TEST_DIR = test/
TEST_INCLUDE = $(GAME_INCLUDE)
TEST_LIBS = -lxml2 -lcheck
TEST_OBJ_DIR = $(OBJ_DIR)$(TEST_DIR)
TEST_SRC_DIR = $(TEST_DIR)src/
TEST_SRC_FILES = $(wildcard $(TEST_SRC_DIR)*.c)
TEST_OBJ_FILES = $(patsubst $(TEST_SRC_DIR)%.c,$(TEST_OBJ_DIR)%.o,$(TEST_SRC_FILES))
TEST_DEP_FILES = $(patsubst $(TEST_SRC_DIR)%.c,$(TEST_OBJ_DIR)%.d,$(TEST_SRC_FILES))

ASSETS_DIR = assets/
ASSETS_MAP_DIR = $(ASSETS_DIR)maps/
ASSETS_TMX_FILES = $(wildcard $(ASSETS_MAP_DIR)*.tmx)
ASSETS_MAP_FILES = $(patsubst $(ASSETS_MAP_DIR)%.tmx,$(GAME_BIN_DIR)$(ASSETS_MAP_DIR)%.map,$(ASSETS_TMX_FILES))

game: $(GAME_BIN_DIR)$(GAME_EXE)

mc: $(MAP_CONVERTER_BIN_DIR)$(MAP_CONVERTER_EXE)

test: $(TEST_BIN_DIR)$(TEST_EXE)

#	Copies assets to app bin - use with -B option
assets: mc $(ASSETS_MAP_FILES)
	mkdir -p -v bin/assets/gfx
	mkdir -p -v bin/assets/sfx
	cp -u -v assets/gfx/* bin/assets/gfx/
	cp -u -v assets/sfx/*.ogg bin/assets/sfx/

clean:
	rm -rf bin/
	rm -rf build/
	rm -rf tools/

$(GAME_BIN_DIR)$(GAME_EXE): $(GAME_OBJ_FILES) $(COMMON_OBJ_FILES)
	$(MKDIR)
	$(LINKER) $(CFLAGS) $^ -o $@ $(GAME_LIBS)

$(MAP_CONVERTER_BIN_DIR)$(MAP_CONVERTER_EXE): $(MAP_CONVERTER_OBJ_FILES) $(COMMON_OBJ_FILES)
	$(MKDIR)
	$(LINKER) $(CFLAGS) $^ -o $@ $(MAP_CONVERTER_LIBS)

$(TEST_BIN_DIR)$(TEST_EXE): $(TEST_OBJ_FILES) $(COMMON_OBJ_FILES)
	$(MKDIR)
	$(LINKER) $(CFLAGS) $^ -o $@ $(TEST_LIBS)

$(COMMON_OBJ_DIR)%.o: $(COMMON_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(COMMON_INCLUDE) -c $< -o $@

$(COMMON_OBJ_DIR)%.d: $(COMMON_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(COMMON_INCLUDE) -MM $< \
		| sed -e 's%^%$@ %' -e 's% % $(COMMON_OBJ_DIR)%'\ > $@

$(GAME_OBJ_DIR)%.o: $(GAME_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(GAME_INCLUDE) -c $< -o $@

$(GAME_OBJ_DIR)%.d: $(GAME_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(GAME_INCLUDE) -MM $< \
		| sed -e 's%^%$@ %' -e 's% % $(GAME_OBJ_DIR)%'\ > $@

$(MAP_CONVERTER_OBJ_DIR)%.o: $(MAP_CONVERTER_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(MAP_CONVERTER_INCLUDE) -c $< -o $@

$(MAP_CONVERTER_OBJ_DIR)%.d: $(MAP_CONVERTER_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(MAP_CONVERTER_INCLUDE) -MM $< \
		| sed -e 's%^%$@ %' -e 's% % $(MAP_CONVERTER_OBJ_DIR)%'\ > $@

$(TEST_OBJ_DIR)%.o: $(TEST_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(CFLAGS) $(TEST_INCLUDE) -c $< -o $@

$(TEST_OBJ_DIR)%.d: $(TEST_SRC_DIR)%.c
	$(MKDIR)
	$(CC) $(TEST_INCLUDE) -MM $< \
		| sed -e 's%^%$@ %' -e 's% % $(TEST_OBJ_DIR)%'\ > $@

$(GAME_BIN_DIR)$(ASSETS_MAP_DIR)%.map: $(ASSETS_MAP_DIR)%.tmx
	$(MKDIR)
	$(MAP_CONVERTER_BIN_DIR)$(MAP_CONVERTER_EXE) $(MAP_CONVERTER_BIN_DIR)../$^ $@