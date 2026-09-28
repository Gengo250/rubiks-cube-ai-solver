#include "BFS_Solver.hpp"
#include "SearchLoop.hpp"

#include <cstdint>
#include <queue>
#include <unordered_set>
#include <utility>

namespace {

constexpr std::size_t MAX_MOV = 10;
constexpr std::size_t MAX_NOS = 2000000;

// Converte a cor para três bits.
std::uint64_t colorToBits(Color color) {
  if (color.r == 230) {
    return 0;
  }
  if (color.r == 200) {
    return 1;
  }
  if (color.r == 235) {
    return 2;
  }
  if (color.r == 240) {
    return 3;
  }
  if (color.r == 45) {
    return 4;
  }
  if (color.r == 40) {
    return 5;
  }
  return 7;
}

// Compacta o estado do cubo em 64 bits.
std::uint64_t getCompactState(const Cube &cube) {
  std::uint64_t state = 0;
  int shift = 0;

  for (const Cubie &cubie : cube.getCubies()) {
    if (cubie.position[0] == 0 &&
        cubie.position[1] == 0 &&
        cubie.position[2] == 0) {
      continue;
    }

    const std::uint64_t position =
        (cubie.position[0] << 2) |
        (cubie.position[1] << 1) |
        cubie.position[2];

    const std::uint64_t colorX = colorToBits(cubie.colors[0]);
    const std::uint64_t colorY = colorToBits(cubie.colors[1]);
    const std::uint64_t pieceData =
        (position << 6) | (colorX << 3) | colorY;

    state |= pieceData << shift;
    shift += 9;
  }

  return state;
}

// A BFS utiliza uma fila: primeiro que entra é o primeiro que sai.
class BFSStructure {
public:
  using NodeType = BFSNode;

  void add(BFSNode node) {
    if (node.path.size() > MAX_MOV ||
        discovered.size() >= MAX_NOS) {
      return;
    }

    const std::uint64_t key = getCompactState(node.state);

    if (discovered.insert(key).second) {
      states.push(std::move(node));
    }
  }

  BFSNode removeNext() {
    BFSNode node = std::move(states.front());
    states.pop();
    return node;
  }

  bool empty() const {
    return states.empty();
  }

private:
  std::queue<BFSNode> states;
  std::unordered_set<std::uint64_t> discovered;
};

std::vector<BFSNode>
generateBFSSuccessors(const BFSNode &current) {
  std::vector<BFSNode> successors;

  if (current.path.size() >= MAX_MOV) {
    return successors;
  }

  for (int axis = 0; axis < 3; ++axis) {
    if (axis == current.ultEixo) {
      continue;
    }

    for (bool clockwise : {true, false}) {
      BFSNode next = current;
      next.state.rotate(axis, 1, clockwise);
      next.path.push_back({axis, 1, clockwise});
      next.ultEixo = axis;

      successors.push_back(std::move(next));
    }
  }

  return successors;
}

} // namespace

std::vector<CubeMove>
solveCubeBFS(Cube initialCube,
             int &qtd_movimentos,
             int &nosvisitados) {
  BFSStructure structure;

  const auto result = executeSearch(
      BFSNode{initialCube, {}, -1},
      structure,
      [](const BFSNode &node) {
        return node.state.isSolved();
      },
      generateBFSSuccessors);

  nosvisitados =
      static_cast<int>(result.visitedStates);

  if (!result.solved()) {
    qtd_movimentos = 0;
    return {};
  }

  qtd_movimentos =
      static_cast<int>(result.finalNode->path.size());

  return result.finalNode->path;
}