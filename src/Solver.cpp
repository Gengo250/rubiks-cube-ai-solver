#include "Solver.hpp"

#include "Heuristic.hpp"

#include <chrono>
#include <unordered_map>
#include <utility>

namespace {

constexpr int MAX_COST = 16;

Turn turnOf(int move) {
  return Turn{heuristic::moveAxis(move), 1, heuristic::moveIsClockwise(move),
              heuristic::moveIsHalf(move)};
}

} // namespace

Solution solveAStar(const Cube &cube) {
  const auto started = std::chrono::steady_clock::now();
  heuristic::build();

  Solution solution;

  // A heuristica so existe com a peca 0 em casa. Reorientar o cubo inteiro
  Cube working = cube;
  for (const heuristic::Spin &spin : heuristic::homingSpins(cube)) {
    working.rotate(spin.axis, 1, spin.clockwise);
    working.rotate(spin.axis, 0, !spin.clockwise);
    solution.setup.push_back(Turn{spin.axis, 1, spin.clockwise, false});
    solution.setup.push_back(Turn{spin.axis, 0, !spin.clockwise, false});
  }

  const heuristic::State start = heuristic::stateOf(working);
  const int goal = heuristic::solvedState().index();

  // Com a heuristica consistente o A* nunca reabre um no fechado, entao um
  // custo ja gravado e definitivo. Sao poucas centenas de nos: mapa em vez
  // de vetor sobre os 11 milhoes de indices.
  std::unordered_map<int, int> cost;
  std::unordered_map<int, std::pair<int, int>> from;
  std::vector<std::vector<heuristic::State>> open(MAX_COST);

  cost[start.index()] = 0;
  open[heuristic::value(start)].push_back(start);

  for (int f = 0; f < MAX_COST && !solution.solved; ++f) {
    while (!open[f].empty()) {
      const heuristic::State state = open[f].back();
      open[f].pop_back();
      const int index = state.index();
      const int g = cost[index];
      if (g + heuristic::value(state) != f) {
        continue; // entrada obsoleta
      }
      if (index == goal) {
        solution.solved = true;
        break;
      }
      ++solution.expanded;

      for (int move = 0; move < heuristic::MOVE_COUNT; ++move) {
        const heuristic::State next = heuristic::apply(state, move);
        const int nextIndex = next.index();
        const auto found = cost.find(nextIndex);
        if (found != cost.end() && found->second <= g + 1) {
          continue;
        }
        cost[nextIndex] = g + 1;
        from[nextIndex] = {index, move};
        const int priority = g + 1 + heuristic::value(next);
        if (priority < MAX_COST) {
          open[priority].push_back(next);
        }
      }
    }
  }

  if (solution.solved) {
    std::vector<int> moves;
    int current = goal;
    while (current != start.index()) {
      const std::pair<int, int> step = from[current];
      moves.push_back(step.second);
      current = step.first;
    }
    for (std::size_t i = moves.size(); i-- > 0;) {
      solution.turns.push_back(turnOf(moves[i]));
      if (!solution.notation.empty()) {
        solution.notation += ' ';
      }
      solution.notation += heuristic::moveName(moves[i]);
    }
  }

  const auto finished = std::chrono::steady_clock::now();
  solution.milliseconds =
      std::chrono::duration<double, std::milli>(finished - started).count();
  return solution;
}
