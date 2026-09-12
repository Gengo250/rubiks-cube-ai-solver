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

# raylib instalada no sistema (Arch: pacote "raylib")
find_package(raylib 6.0 REQUIRED)

add_library(rubiks_cube_core
    src/Cube.cpp
    src/CubeRenderer.cpp
    src/CubeSnapshot.cpp
    src/Menu.cpp
    src/MenuRenderer.cpp
)

target_include_directories(rubiks_cube_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PRIVATE: apenas MenuRenderer.cpp usa a raylib, os headers do projeto nao a expoem.
target_link_libraries(rubiks_cube_core
    PRIVATE
        raylib
)

add_executable(rubiks_cube_ai_solver
    src/main.cpp
)

target_link_libraries(rubiks_cube_ai_solver
    PRIVATE
        rubiks_cube_core
)

# O menu carrega a fonte de "assets/fonts" ao lado do executavel.
add_custom_command(TARGET rubiks_cube_ai_solver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:rubiks_cube_ai_solver>/assets
    COMMENT "Copiando assets/ para a pasta do executavel"
)
