#pragma once

#include "Cube.hpp"

class Menu {
public:
  Menu();
  ~Menu();

  Menu(const Menu &) = delete;
  Menu &operator=(const Menu &) = delete;
  void run();

private:
  int readChoice();
  void draw() const;
  void drawCube() const;
  void drawText(const char *text, int x, int y, int size, Color color) const;

  Cube cube;
  Font font{};
  int selected = 0;
  bool cubeVisible = false;
  const char *statusMessage = "";
};
