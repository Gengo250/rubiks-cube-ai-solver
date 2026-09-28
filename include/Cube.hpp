#pragma once

#include <array>
#include <raylib.h>

struct Cubie {
  // Eixos X, Y, Z: cada coordenada vale 0 ou 1.
  std::array<int, 3> position;
  // Adesivo voltado para fora em cada eixo; o lado vem da posicao.
  std::array<Color, 3> colors;
};

class Cube {
public:
  Cube();

  const std::array<Cubie, 8> &getCubies() const { return cubies; }
  // axis: 0=X, 1=Y, 2=Z; layer: 0 ou 1.
  // Horario olhando a camada de fora do cubo.
  void rotate(int axis, int layer, bool clockwise);
  void shuffle();
  bool isSolved() const;

private:
  std::array<Cubie, 8> cubies;
};