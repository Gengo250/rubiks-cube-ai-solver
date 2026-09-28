#include "DepthFirstSearch.hpp"
#include "SearchLoop.hpp"

#include <algorithm>
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

void appendColor(std::string &key,
                 const Color &color) {
  key.push_back(static_cast<char>(color.r));
  key.push_back(static_cast<char>(color.g));
  key.push_back(static_cast<char>(color.b));
  key.push_back(static_cast<char>(color.a));
}

bool areInverseMoves(const DepthMove &first,
                     const DepthMove &second) {
  return first.axis == second.axis &&
         first.layer == second.layer &&
         first.clockwise != second.clockwise;
}

// O IDDFS utiliza uma pilha limitada.
class DepthStructure {
public:
  using NodeType = SearchNode;

  explicit DepthStructure(int depthLimit)
      : depthLimit(depthLimit) {
  }

  void add(SearchNode node) {
    if (node.depth > depthLimit) {
      return;
    }

    const int remainingDepth =
        depthLimit - node.depth;

    const std::string key =
        DepthFirstSearch::createStateKey(node.state);

    const auto found =
        bestRemainingDepth.find(key);

    if (found != bestRemainingDepth.end() &&
        found->second >= remainingDepth) {
      return;
    }

    bestRemainingDepth[key] = remainingDepth;
    states.push_back(std::move(node));
  }

  SearchNode removeNext() {
    SearchNode node = std::move(states.back());
    states.pop_back();
    return node;
  }

  bool empty() const {
    return states.empty();
  }

private:
  int depthLimit;
  std::vector<SearchNode> states;
  std::unordered_map<std::string, int>
      bestRemainingDepth;
};

std::vector<SearchNode>
generateDepthSuccessors(const SearchNode &current) {
  std::vector<SearchNode> successors;

  for (const DepthSuccessor &successor :
       DepthFirstSearch::generateSuccessors(
           current.state)) {
    if (!current.path.empty() &&
        areInverseMoves(
            current.path.back(),
            successor.move)) {
      continue;
    }

    std::vector<DepthMove> nextPath =
        current.path;

    nextPath.push_back(successor.move);

    successors.push_back({
        successor.state,
        std::move(nextPath),
        current.depth + 1,
    });
  }

  // Como a estrutura é uma pilha, a inversão preserva
  // a ordem original dos movimentos.
  std::reverse(
      successors.begin(),
      successors.end());

  return successors;
}

} // namespace

const std::array<DepthMove, 12> &
DepthFirstSearch::getPossibleMoves() {
  return POSSIBLE_MOVES;
}

std::vector<DepthSuccessor>
DepthFirstSearch::generateSuccessors(
    const Cube &state) {
  std::vector<DepthSuccessor> successors;
  successors.reserve(POSSIBLE_MOVES.size());

  for (const DepthMove &move : POSSIBLE_MOVES) {
    Cube nextState = state;

    nextState.rotate(
        move.axis,
        move.layer,
        move.clockwise);

    successors.push_back({
        nextState,
        move,
    });
  }

  return successors;
}

std::string
DepthFirstSearch::createStateKey(
    const Cube &state) {
  std::string key;
  key.reserve(8 * 15);

  for (const Cubie &cubie :
       state.getCubies()) {
    for (int coordinate :
         cubie.position) {
      key.push_back(
          static_cast<char>(coordinate));
    }

    for (const Color &color :
         cubie.colors) {
      appendColor(key, color);
    }
  }

  return key;
}

DepthSearchResult
DepthFirstSearch::depthLimitedSearch(
    const Cube &initialState,
    int depthLimit) {
  if (depthLimit < 0) {
    throw std::invalid_argument(
        "O limite de profundidade nao pode ser negativo");
  }

  DepthStructure structure(depthLimit);

  const auto commonResult = executeSearch(
      SearchNode{initialState, {}, 0},
      structure,
      [](const SearchNode &node) {
        return node.state.isSolved();
      },
      generateDepthSuccessors);

  DepthSearchResult result;

  result.visitedStates =
      commonResult.visitedStates;

  if (commonResult.solved()) {
    result.solved = true;

    result.solutionDepth =
        commonResult.finalNode->depth;

    result.solution =
        std::move(
            commonResult.finalNode->path);
  }

  return result;
}

DepthSearchResult
DepthFirstSearch::iterativeDeepeningSearch(
    const Cube &initialState,
    int maximumDepth) {
  if (maximumDepth < 0) {
    throw std::invalid_argument(
        "A profundidade maxima nao pode ser negativa");
  }

  DepthSearchResult finalResult;

  for (int limit = 0;
       limit <= maximumDepth;
       ++limit) {
    DepthSearchResult currentResult =
        depthLimitedSearch(
            initialState,
            limit);

    finalResult.visitedStates +=
        currentResult.visitedStates;

    if (currentResult.solved) {
      currentResult.visitedStates =
          finalResult.visitedStates;

      return currentResult;
    }
  }

  return finalResult;
}