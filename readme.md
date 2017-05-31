# Aventail

A small game written in C with Linux.  

![screenshot](/screenshot.gif?raw=true)

# Map Converter

Converts [Tiled](http://www.mapeditor.org/) TMX files to the format used by the game.

# Dependencies

Game
* SDL2
* SDL2_image
* SDL2_mixer

Map Converter
* libxml2

Test
* [Unity](http://www.throwtheswitch.org/unity/)

# Directory Structure

assets - game assets (audio, image, and map files)  
common - common source and header files  
game - game source and header files  
map_converter - map converter source and header files  
test - [Unity](http://www.throwtheswitch.org/unity/) tests  

# Build

A makefile is included.

To build the game:
```
make game
```

To build the map converter:
```
make mc
```

To build and copy assets to bin/game:
```
make assets
```

To remove the bin, build and tools directories.
```
make clean
```

Several directories are created by the makefile:  
bin: Contains the game executable and assets.  
bin_tests: Contains the unit test executables.  
tools: Contains the map converter executable.  
build: Contains object files.  

The TMX files in the assets/maps directory will be converted to MAP format and placed in the bin/assets/maps directory.

CMake files are also included but these are still new.  
The tests are built with CMake not the makefile.
The assets are not built by CMake yet.  

