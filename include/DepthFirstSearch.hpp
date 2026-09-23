#pragma once

#include "Cube.hpp"

#include <array>
#include <string>
#include <vector>

struct Move {
  std::string name;
  int axis;
  int layer;
  bool clockwise;
};

struct Successor {
  Cube state;
  Move move;
};

class DepthFirstSearch {
public:
  static const std::array<Move, 12> &getPossibleMoves();
  static std::vector<Successor> generateSuccessors(const Cube &state);
  static std::string createStateKey(const Cube &state);
};