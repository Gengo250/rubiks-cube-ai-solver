#include "Cube.hpp"x
#include <cstddef>

Cube::Cube() {
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

void Cube::moveFrontClockwise() {
  std::size_t front = static_cast<std::size_t>(Face::Front);
  std::size_t upper = static_cast<std::size_t>(Face::Upper);
  std::size_t lower = static_cast<std::size_t>(Face::Lower);
  std::size_t right = static_cast<std::size_t>(Face::Right);
  std::size_t left = static_cast<std::size_t>(Face::Left);

  // Gira a própria face frontal no sentido horário.
  auto oldFront = faces[front];

  faces[front][0] = oldFront[2];
  faces[front][1] = oldFront[0];
  faces[front][2] = oldFront[3];
  faces[front][3] = oldFront[1];

  // Guarda a parte inferior da face superior.
  Color upperBottomLeft = faces[upper][2];
  Color upperBottomRight = faces[upper][3];

  // Esquerda -> Superior
  faces[upper][2] = faces[left][3];
  faces[upper][3] = faces[left][1];

  // Inferior -> Esquerda
  faces[left][1] = faces[lower][0];
  faces[left][3] = faces[lower][1];

  // Direita -> Inferior
  faces[lower][0] = faces[right][2];
  faces[lower][1] = faces[right][0];

  // Superior -> Direita
  faces[right][0] = upperBottomLeft;
  faces[right][2] = upperBottomRight;
}