#include "Solver.hpp"

#include "Heuristic.hpp"
#include "SearchLoop.hpp"

#include <chrono>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr int MAX_COST = 16;

Turn turnOf(int move) {
  return Turn{
      heuristic::moveAxis(move),
      1,
      heuristic::moveIsClockwise(move),
      heuristic::moveIsHalf(move),
  };
}

struct AStarNode {
  heuristic::State state;
  int cost = 0;
  int parent = -1;
  int move = -1;
};

struct CompareAStarNode {
  bool operator()(const AStarNode &first,
                  const AStarNode &second) const {
    return first.cost +
               heuristic::value(first.state) >
           second.cost +
               heuristic::value(second.state);
  }
};

// O A* utiliza uma fila de prioridade ordenada por f = g + h.
class AStarStructure {
public:
  using NodeType = AStarNode;

  explicit AStarStructure(
      std::unordered_map<
          int,
          std::pair<int, int>> &parents)
      : parents(parents) {
  }

  void add(AStarNode node) {
    const int index =
        node.state.index();

    const int priority =
        node.cost +
        heuristic::value(node.state);

    const auto found =
        bestCost.find(index);

    if (priority >= MAX_COST ||
        (found != bestCost.end() &&
         found->second <= node.cost)) {
      return;
    }

    bestCost[index] = node.cost;

    if (node.move >= 0) {
      parents[index] = {
          node.parent,
          node.move,
      };
    }

    states.push(std::move(node));
  }

  AStarNode removeNext() {
    discardObsolete();

    AStarNode node = states.top();
    states.pop();

    return node;
  }

  bool empty() {
    discardObsolete();
    return states.empty();
  }

private:
  void discardObsolete() {
    while (!states.empty()) {
      const AStarNode &node =
          states.top();

      const auto found =
          bestCost.find(
              node.state.index());

      if (found != bestCost.end() &&
          found->second == node.cost) {
        break;
      }

      states.pop();
    }
  }

  std::priority_queue<
      AStarNode,
      std::vector<AStarNode>,
      CompareAStarNode>
      states;

  std::unordered_map<int, int>
      bestCost;

  std::unordered_map<
      int,
      std::pair<int, int>> &parents;
};

std::vector<AStarNode>
generateAStarSuccessors(
    const AStarNode &current) {
  std::vector<AStarNode> successors;

  successors.reserve(
      heuristic::MOVE_COUNT);

  for (int move = 0;
       move < heuristic::MOVE_COUNT;
       ++move) {
    successors.push_back({
        heuristic::apply(
            current.state,
            move),
        current.cost + 1,
        current.state.index(),
        move,
    });
  }

  return successors;
}

} // namespace

Solution solveAStar(const Cube &cube) {
  const auto started =
      std::chrono::steady_clock::now();

  heuristic::build();

  Solution solution;

 
  Cube working = cube;

  for (const heuristic::Spin &spin :
       heuristic::homingSpins(cube)) {
    working.rotate(
        spin.axis,
        1,
        spin.clockwise);

    working.rotate(
        spin.axis,
        0,
        !spin.clockwise);

    solution.setup.push_back({
        spin.axis,
        1,
        spin.clockwise,
        false,
    });

    solution.setup.push_back({
        spin.axis,
        0,
        !spin.clockwise,
        false,
    });
  }

  const heuristic::State start =
      heuristic::stateOf(working);

  const int goal =
      heuristic::solvedState().index();

  std::unordered_map<
      int,
      std::pair<int, int>>
      parents;

  AStarStructure structure(parents);

  const auto result = executeSearch(
      AStarNode{
          start,
          0,
          -1,
          -1,
      },
      structure,
      [goal](const AStarNode &node) {
        return node.state.index() == goal;
      },
      generateAStarSuccessors);

  solution.solved = result.solved();

  solution.expanded =
      static_cast<long long>(
          result.visitedStates);

  if (solution.solved) {
    std::vector<int> moves;
    int current = goal;

    while (current != start.index()) {
      const std::pair<int, int> step =
          parents.at(current);

      moves.push_back(step.second);
      current = step.first;
    }

    for (auto iterator = moves.rbegin();
         iterator != moves.rend();
         ++iterator) {
      solution.turns.push_back(
          turnOf(*iterator));

      if (!solution.notation.empty()) {
        solution.notation += ' ';
      }

      solution.notation +=
          heuristic::moveName(*iterator);
    }
  }

  const auto finished =
      std::chrono::steady_clock::now();

  solution.milliseconds =
      std::chrono::duration<
          double,
          std::milli>(
          finished - started)
          .count();

  return solution;
}