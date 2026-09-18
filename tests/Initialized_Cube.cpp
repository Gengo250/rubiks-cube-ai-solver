#include "Initialized_Cube.hpp"
#include "Cube.hpp"
#include <iostream>
#include <cassert>

void testInicializedCube() {
  Cube cube;

  assert(cube.getStickerColor(0, 1, 0, 'Y') == Color::White);
  assert(cube.getStickerColor(0, 0, 0, 'Y') == Color::Yellow);
  assert(cube.getStickerColor(0, 0, 1, 'Z') == Color::Green);
  assert(cube.getStickerColor(0, 0, 0, 'Z') == Color::Blue);
  assert(cube.getStickerColor(1, 0, 0, 'X') == Color::Red);
  assert(cube.getStickerColor(0, 0, 0, 'X') == Color::Orange);
  
  std::cout << "Cube initialized correctly\n";

}
