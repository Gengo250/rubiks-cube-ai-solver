#include "Menu.hpp"

#include "CubeRenderer.hpp"
#include "Initialized_Cube.hpp"

#include <iostream>

Menu::Menu() {}

void Menu::menu_text() {
  std::cout << '\n';
  std::cout << "MENU - CUBE 2X2X2" << '\n';
  std::cout << "0 - Finish" << '\n';
  std::cout << "1 - Render Cube" << '\n';
  std::cout << "2 - Test initialized Cube" << '\n';
  std::cout << "3 - Move Front Clockwise" << '\n';
  std::cout << "4 - Move Left Clockwise" << '\n';
  std::cout << "5 - Move Front Counterclockwise" << '\n';
  std::cout << "6 - Move Right Clockwise" << '\n';
  std::cout << "7 - Move Right Counterclockwise " << '\n';

  std::cout << "Choice: ";
}

void Menu::inicializated_Menu(int index) {
  switch (index) {
    case 0:
      std::cout << "Finish..." << '\n';
      break;

    case 1:
      CubeRenderer::render(cube);
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
      cube.moveLeftClockwise();
      std::cout << "Left face moved clockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 5:
      cube.moveFrontCounterclockwise();
      std::cout << "Front face moved Counterclockwise" << '\n';
      CubeRenderer::render(cube);
      break;
    case 6:
      cube.moveRightClockwise();
      std::cout << "Right face moved clockwise"  << '\n';
      CubeRenderer::render(cube);
      break;
    case 7:
      cube.moveRightCounterclockwise();
      std::cout << "Right face moved Counterclockwise" << '\n';
      CubeRenderer::render(cube);
      break;
    default:
      std::cout << "Invalid option!" << '\n';
      break;
  }
}
