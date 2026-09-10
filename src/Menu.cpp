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
    case 8:
      cube.moveLeftCounterclockwise();
      std::cout << "Left face moved Counterclockwise"  << '\n';
      CubeRenderer::render(cube);
      break;
    case 9:
      cube.moveTopClockwise();
      std::cout << "Top face moved clockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 10:
      cube.moveTopCounterclockwise();
      std::cout << "Top face moved couterclockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 11:
      cube.moveLowerClockwise();
      std::cout << "Lower face moved clockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 12:
      cube.moveLowerCounterclockwise();
      std::cout << "Lower face moved counterclockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 13:
      cube.MoveRearClockwise();
      std::cout << "Rear face moved clockwise." << '\n';
      CubeRenderer::render(cube);
      break;
    case 14:
      cube.MoveRearCounterClockwise();
      std::cout << "Rear face moved counterclockwise." << '\n';
      CubeRenderer::render(cube);
      break;

      
    default:
      std::cout << "Invalid option!" << '\n';
      break;
  }
}
