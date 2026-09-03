#pragma once

#include "Cube.hpp"

class Menu {
 public:
  Menu();

  void menu_text();
  void inicializated_Menu(int index);

 private:
  Cube cube;
};