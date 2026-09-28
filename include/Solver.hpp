#pragma once

#include "Cube.hpp"

#include <string>
#include <vector>

// Um giro aplicavel em Cube::rotate. Meia-volta conta um movimento HTM e
// vale duas chamadas de rotate.
struct Turn {
  int axis = 0;
  int layer = 1;
  bool clockwise = true;
  bool half = false;
};

struct Solution {
  bool solved = false;
  // Reorientacao do cubo inteiro para o modelo de canto fixo. Nao conta como
  // movimento da solucao.
  std::vector<Turn> setup;
  // Solucao otima, um Turn por movimento HTM.
  std::vector<Turn> turns;
  std::string notation;
  long long expanded = 0;
  double milliseconds = 0.0;
};

// A* com a heuristica de docs/heuristica-a-estrela.md. Nao altera o cubo
// recebido: devolve os giros que o chamador aplica.
Solution solveAStar(const Cube &cube);
