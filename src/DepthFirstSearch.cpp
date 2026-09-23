#include "DepthFirstSearch.hpp"

namespace {

const std::array<Move, 12> POSSIBLE_MOVES = {{
    {"Front clockwise", 2, 1, true},
    {"Front counterclockwise", 2, 1, false},
    {"Rear clockwise", 2, 0, true},
    {"Rear counterclockwise", 2, 0, false},
    {"Right clockwise", 0, 1, true},
    {"Right counterclockwise", 0, 1, false},
    {"Left clockwise", 0, 0, true},
    {"Left counterclockwise", 0, 0, false},
    {"Top clockwise", 1, 1, true},
    {"Top counterclockwise", 1, 1, false},
    {"Lower clockwise", 1, 0, true},
    {"Lower counterclockwise", 1, 0, false},
}};

void appendColor(std::string &key, const Color &color) {
  key.push_back(static_cast<char>(color.r));
  key.push_back(static_cast<char>(color.g));
  key.push_back(static_cast<char>(color.b));
  key.push_back(static_cast<char>(color.a));
}

} // namespace

const std::array<Move, 12> &DepthFirstSearch::getPossibleMoves() {
  return POSSIBLE_MOVES;
}

std::vector<Successor>
DepthFirstSearch::generateSuccessors(const Cube &state) {
  std::vector<Successor> successors;
  successors.reserve(POSSIBLE_MOVES.size());

  for (const Move &move : POSSIBLE_MOVES) {
    Cube nextState = state;
    nextState.rotate(move.axis, move.layer, move.clockwise);

    successors.push_back({
        nextState,
        move,
    });
  }

  return successors;
}

std::string DepthFirstSearch::createStateKey(const Cube &state) {
  std::string key;
  key.reserve(8 * 15);

  for (const Cubie &cubie : state.getCubies()) {
    for (int coordinate : cubie.position) {
      key.push_back(static_cast<char>(coordinate));
    }

    for (const Color &color : cubie.colors) {
      appendColor(key, color);
    }
  }

  return key;
}