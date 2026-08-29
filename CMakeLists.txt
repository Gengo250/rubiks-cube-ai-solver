cmake_minimum_required(VERSION 3.20)

project(
    rubiks_cube_ai_solver
    VERSION 0.1.0
    DESCRIPTION "Interactive 2x2x2 Rubik's Cube simulator and AI solver"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(rubiks_cube_core
    src/Cube.cpp
)

target_include_directories(rubiks_cube_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(rubiks_cube_ai_solver
    src/main.cpp
)

target_link_libraries(rubiks_cube_ai_solver
    PRIVATE
        rubiks_cube_core
)
