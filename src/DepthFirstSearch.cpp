#include "DepthFirstSearch.hpp"
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace {

const std::array<DepthMove, 12> POSSIBLE_MOVES = {{
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

struct SearchNode {
  Cube state;
  std::vector<DepthMove> path;
  int depth;
};

void appendColor(std::string &key, const Color &color) {
  key.push_back(static_cast<char>(color.r));
  key.push_back(static_cast<char>(color.g));
  key.push_back(static_cast<char>(color.b));
  key.push_back(static_cast<char>(color.a));
}

bool areInverseMoves(const DepthMove &first, const DepthMove &second) {
  return first.axis == second.axis && first.layer == second.layer &&
         first.clockwise != second.clockwise;
}

} // namespace

const std::array<DepthMove, 12> &DepthFirstSearch::getPossibleMoves() {
  return POSSIBLE_MOVES;
}

std::vector<DepthSuccessor>
DepthFirstSearch::generateSuccessors(const Cube &state) {
  std::vector<DepthSuccessor> successors;
  successors.reserve(POSSIBLE_MOVES.size());

  for (const DepthMove &move : POSSIBLE_MOVES) {
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

DepthSearchResult
DepthFirstSearch::depthLimitedSearch(const Cube &initialState,
                                     int depthLimit) {
  if (depthLimit < 0) {
    throw std::invalid_argument(
        "O limite de profundidade nao pode ser negativo");
  }

  DepthSearchResult result;
  std::vector<SearchNode> stack;
  stack.push_back({initialState, {}, 0});

  std::unordered_map<std::string, int> bestRemainingDepth;
  bestRemainingDepth[createStateKey(initialState)] = depthLimit;

  while (!stack.empty()) {
    SearchNode current = std::move(stack.back());
    stack.pop_back();
    ++result.visitedStates;

    if (current.state.isSolved()) {
      result.solved = true;
      result.solutionDepth = current.depth;
      result.solution = std::move(current.path);
      return result;
    }

    if (current.depth >= depthLimit) {
      continue;
    }

    std::vector<DepthSuccessor> successors = generateSuccessors(current.state);
    for (auto iterator = successors.rbegin(); iterator != successors.rend();
         ++iterator) {
      DepthSuccessor &successor = *iterator;

      if (!current.path.empty() &&
          areInverseMoves(current.path.back(), successor.move)) {
        continue;
      }

      const int nextDepth = current.depth + 1;
      const int remainingDepth = depthLimit - nextDepth;
      const std::string key = createStateKey(successor.state);
      const auto found = bestRemainingDepth.find(key);

      if (found != bestRemainingDepth.end() &&
          found->second >= remainingDepth) {
        continue;
      }

      bestRemainingDepth[key] = remainingDepth;
      std::vector<DepthMove> nextPath = current.path;
      nextPath.push_back(successor.move);
      stack.push_back(
          {std::move(successor.state), std::move(nextPath), nextDepth});
    }
  }

  return result;
}

DepthSearchResult
DepthFirstSearch::iterativeDeepeningSearch(const Cube &initialState,
                                           int maximumDepth) {
  if (maximumDepth < 0) {
    throw std::invalid_argument(
        "A profundidade maxima nao pode ser negativa");
  }

  DepthSearchResult finalResult;
  for (int limit = 0; limit <= maximumDepth; ++limit) {
    DepthSearchResult currentResult = depthLimitedSearch(initialState, limit);
    finalResult.visitedStates += currentResult.visitedStates;

    if (currentResult.solved) {
      currentResult.visitedStates = finalResult.visitedStates;
      return currentResult;
    }
  }

  return finalResult;
}