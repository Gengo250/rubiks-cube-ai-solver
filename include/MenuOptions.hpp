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
    {2, "Test initialized Cube"},
    {3, "Move Front Clockwise"},
    {4, "Move Front Counterclockwise"},
    {5, "Move Right Clockwise"},
    {6, "Move Right Counterlockwise"},
    {7, "Move Upper Counterclockwise"},
    {8, "Move Upper Clockwise"},
}};
