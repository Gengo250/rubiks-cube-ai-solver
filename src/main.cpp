#include "Menu.hpp"
#include <iostream>

int main (int argc, char *argv[]) {
  Menu menu;

  int index = 0;

  do {
    menu.menu_text();
    std::cin >> index;
    menu.inicializated_Menu(index);

  }while (index);

  return 0;
}
