#include "Menu.hpp"
#include "Cube.hpp"
#include "CubeRenderer.hpp"
#include "Initialized_Cube.hpp"
#include <cassert>
#include <iostream>


Menu::Menu() {}

void Menu::menu_text(){
  std::cout << "MENU - CUBE 2X2X2" << '\n';
  std::cout << "0 - Finish" << '\n';
  std::cout << "1 - Render Cube" << '\n';
  std::cout << "2 - Test inicializated Cube" << '\n';
  std::cout << "Choice: " << '\n';
  
}

void Menu::inicializated_Menu(int index){
  switch (index) {
    case 0:
      std::cout << "Finish ..." << '\n';
      break;
    case 1: {
     Cube cube;
     CubeRenderer::render(cube);
     break;
    }
    case 2:
      testInicializedCube();
      break;
    default:
      std::cout << "!";
      break;
  }
}
