#pragma once

#include <array>
#include <cstddef>

// Uma opcao do menu: o indice que vai para Menu::inicializated_Menu
// e o texto mostrado no terminal e na janela raylib.
struct MenuOption {
  int index;
  const char* label;
};

inline constexpr std::size_t MENU_OPTION_COUNT = 15;

// Fonte unica de verdade do menu.
// Para adicionar um movimento novo: crie o metodo em Cube, trate o novo
// indice em Menu::inicializated_Menu e acrescente uma linha aqui.
inline constexpr std::array<MenuOption, MENU_OPTION_COUNT> MENU_OPTIONS = {{
    {0, "Finish"},
    {1, "Render Cube"},
    {2, "Shuffle Cube"},
    {3, "Move Front Clockwise"},
    {4, "Move Left Clockwise"},
    {5, "Move Front Counterclockwise"},
    {6, "Move Right Clockwise"},
    {7, "Move Right Counterclockwise"},
    {8, "Move Left Counterclockwise"},
    {9, "Move Top Clockwise"},
    {10, "Move Top Counterclockwise"},
    {11, "Move Lower Clockwise"},
    {12, "Move Lower Counterclockwise"},
    {13, "Move Rear Clockwise"},
    {14, "Move Rear Counterclockwise"},
}};
