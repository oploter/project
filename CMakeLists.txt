cmake_minimum_required(VERSION 3.10)

project(project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SFML REQUIRED system graphics window network)
include(default-options.cmake)
# include_directories(include)
add_executable(project main.cpp view.cpp model.cpp map.cpp player.cpp)
target_compile_options(project PUBLIC -Wall -Wextra -Werror -std=c++17)
target_link_libraries(project sfml-graphics sfml-system sfml-window sfml-network)