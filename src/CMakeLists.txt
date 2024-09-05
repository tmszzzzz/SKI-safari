cmake_minimum_required(VERSION 3.23)
project(SKI C)

set(CMAKE_C_STANDARD 99)

add_executable(SKI WIN32 SKI.c src/objects/EQU.c src/objects/OBS.c src/objects/PLR.c src/objects/DEC.c src/objects/SNC.c)
target_link_libraries(SKI mingw32)
target_link_libraries(SKI SDL2)
target_link_libraries(SKI SDLmain)
target_link_libraries(SKI SDL2_ttf)
target_link_libraries(SKI SDL2_image)
