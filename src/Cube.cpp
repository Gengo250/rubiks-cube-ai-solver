#include "Cube.hpp"
#include <cstddef>

Cube::Cube(){
  auto upper = static_cast<std::size_t>(Face::Upper);
  faces[upper].fill(Color::White);

  auto lower = static_cast<std::size_t>(Face::Lower);
  faces[lower].fill(Color::Yellow);

  auto front = static_cast<std::size_t>(Face::Front);
  faces[front].fill(Color::Green);

  auto rear = static_cast<std::size_t>(Face::Rear);
  faces[rear].fill(Color::Blue);

  auto right = static_cast<std::size_t>(Face::Right);
  faces[right].fill(Color::Red);

  auto left = static_cast<std::size_t>(Face::Left);
  faces[left].fill(Color::Orange);
}

Color Cube::getColor(Face face, std::size_t position) const {
  std::size_t faceIndex = static_cast<std::size_t>(face);

  return faces[faceIndex][position];
}
