#include "Cube.hpp"

#include <random>
#include <stdexcept>
#include <utility>

Cube::Cube() {
  // Pares de cores nos lados 0 e 1 dos eixos X, Y e Z.
  constexpr Color colors[3][2] = {
      {{230, 130, 40, 255}, {200, 55, 55, 255}},   // Laranja, vermelho
      {{235, 200, 40, 255}, {240, 240, 240, 255}}, // Amarelo, branco
      {{45, 105, 200, 255}, {40, 170, 90, 255}},   // Azul, verde
  };

  int i = 0;
  for (int x = 0; x < 2; ++x) {
    for (int y = 0; y < 2; ++y) {
      for (int z = 0; z < 2; ++z) {
        cubies[i++] = {{x, y, z}, {colors[0][x], colors[1][y], colors[2][z]}};
      }
    }
  }
}

void Cube::rotate(int axis, int layer, bool clockwise) {
  if (axis < 0 || axis > 2 || layer < 0 || layer > 1) {
    throw std::invalid_argument("Eixo deve ser 0..2 e camada deve ser 0..1");
  }

  const int a = (axis + 1) % 3;
  const int b = (axis + 2) % 3;
  const bool forward = clockwise == (layer == 1);

  for (Cubie &cubie : cubies) {
    if (cubie.position[axis] != layer) {
      continue;
    }

    const int oldA = cubie.position[a];
    cubie.position[a] = forward ? cubie.position[b] : 1 - cubie.position[b];
    cubie.position[b] = forward ? 1 - oldA : oldA;
    std::swap(cubie.colors[a], cubie.colors[b]);
  }
}

void Cube::shuffle() {
  static std::mt19937 engine{std::random_device{}()};
  std::uniform_int_distribution<int> axis(0, 2);
  std::uniform_int_distribution<int> bit(0, 1);

  for (int i = 0; i < 20; ++i) {
    rotate(axis(engine), bit(engine), bit(engine) == 1);
  }
}
