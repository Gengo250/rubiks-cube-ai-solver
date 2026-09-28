#pragma once

#include "Cube.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <vector>

struct DepthMove {
  std::string name;
  int axis;
  int layer;
  bool clockwise;
};

struct DepthSuccessor {
  Cube state;
  DepthMove move;
};

struct DepthSearchResult {
  bool solved = false;
  std::size_t visitedStates = 0;
  int solutionDepth = -1;
  std::vector<DepthMove> solution;
};

class DepthFirstSearch {
public:
  static const std::array<DepthMove, 12> &getPossibleMoves();
  static std::vector<DepthSuccessor> generateSuccessors(const Cube &state);
  static std::string createStateKey(const Cube &state);
  static DepthSearchResult depthLimitedSearch(const Cube &initialState,
                                              int depthLimit);
  static DepthSearchResult iterativeDeepeningSearch(const Cube &initialState,
                                                    int maximumDepth);

};