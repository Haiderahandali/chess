cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(GAME_BOOK)


include_directories(/usr/local/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)





add_executable(Game 
	src/main.cpp)





#finding the libraries 
add_library(DEPS INTERFACE)
find_library(SDL2 SDL2 REQUIRED)

find_library(SDL2main SDL2main REQUIRED)
find_library(SDL2_image SDL2_image REQUIRED)
find_library(SDL2_ttf SDL2_ttf REQUIRED)

target_link_libraries(DEPS INTERFACE ${SDL_image} INTERFACE ${SDL2main} INTERFACE ${SDL2})

#Making it output to bin/Deubg or bin/Release Depending on which config we are using
set_target_properties(Game PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/debug RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/release)
#
# set_target_properties(Engine PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/release)
# set_target_properties(Engine PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib/debug ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib/release)


# target_compile_options(Engine PUBLIC -Wall -Wextra -pedantic -Werror -m64 -std=c++17)
target_compile_options(Game PUBLIC -Wall -Wextra -pedantic -Werror -m64 -std=c++17)
# target_link_libraries(Engine PUBLIC ${SDL2} PUBLIC ${SDL2main}  PUBLIC ${SDL2_image} PUBLIC ${SDL2_ttf})
target_link_libraries(Game PUBLIC DEPS)