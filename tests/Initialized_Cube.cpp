#include "Cube.hpp"
#include <iostream>
#include <cassert>

int main (int argc, char *argv[]) {
  Cube cube;

  assert(cube.getColor(Face::Upper,0) == Color::White);
  assert(cube.getColor(Face::Lower,0) == Color::Yellow);
  assert(cube.getColor(Face::Front,0) == Color::Green);
  assert(cube.getColor(Face::Rear,0) == Color::Blue);
  assert(cube.getColor(Face::Right,0) == Color::Red);
  assert(cube.getColor(Face::Left,0) == Color::Orange);
  
  std::cout << "Cube initialized correctly\n";

  return 0;
}
