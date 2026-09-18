#include "Menu.hpp"

#include "CubeRenderer.hpp"
#include "Initialized_Cube.hpp"
#include "MenuOptions.hpp"

#include <iostream>

Menu::Menu() {}

void Menu::menu_text() {
  std::cout << '\n';
  std::cout << "MENU - CUBE 2X2X2" << '\n';

  for (const MenuOption& option : MENU_OPTIONS) {
    std::cout << option.index << " - " << option.label << '\n';
  }

  std::cout << "Choice: ";
}

void Menu::inicializated_Menu(int index) {
  switch (index) {
    case 0:
      std::cout << "Finish..." << '\n';
      break;

    case 1:
      // O desenho acontece na janela (painel direito do MenuRenderer),
      // por isso esta opcao nao imprime nada no terminal.
      break;

    case 2:
      testInicializedCube();
      break;

    case 3:
      cube.moveFrontClockwise();
      std::cout << "Front face moved clockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 4:
      cube.moveFrontCounterclockwise();
      std::cout << "Front face moved Counterclockwise" << '\n';
      CubeRenderer::render(cube);
      break;
    case 5:
      cube.moveRightClockwise();
      std::cout << "Right face moved clockwise"  << '\n';
      CubeRenderer::render(cube);
      break;
    case 6:
      cube.moveRightCounterclockwise();
      std::cout << "Right face moved Counterclockwise" << '\n';
      CubeRenderer::render(cube);
      break;
    case 7:
      cube.moveUpperClockwise();
      std::cout << "Top face moved clockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 8:
      cube.moveUpperCounterclockwise();
      std::cout << "Top face moved couterclockwise." << '\n';
      CubeRenderer::render(cube);
      break;

    default:
      std::cout << "Invalid option!" << '\n';
      break;
  }
}
