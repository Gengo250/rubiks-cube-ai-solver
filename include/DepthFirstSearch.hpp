#pragma once

#include "Cube.hpp"

#include <array>
#include <cstddef>
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

struct SearchResult {
  bool solved = false;
  std::size_t visitedStates = 0;
  int solutionDepth = -1;
  std::vector<Move> solution;
};

class DepthFirstSearch {
public:
  static const std::array<Move, 12> &getPossibleMoves();
  static std::vector<Successor> generateSuccessors(const Cube &state);
  static std::string createStateKey(const Cube &state);
  static SearchResult depthLimitedSearch(const Cube &initialState,
                                         int depthLimit);
  static SearchResult iterativeDeepeningSearch(const Cube &initialState,
                                               int maximumDepth);
};
