#pragma once

#include "Cube.hpp"

class Menu {
 public:
  Menu();

  void menu_text();
  void inicializated_Menu(int index);

  // Estado atual do cubo, para quem so precisa ler (ex.: o painel grafico).
  const Cube& getCube() const { return cube; }

 private:
  Cube cube;
};