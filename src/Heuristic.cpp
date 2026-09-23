#include "Heuristic.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <queue>

namespace heuristic {
namespace {

constexpr int PERM_COUNT = 5040;
constexpr int ORI_COUNT = 2187;
constexpr int MOVABLE = 7;

constexpr std::array<int, 3> GROUP_A = {1, 2, 3};
constexpr std::array<int, 4> GROUP_B = {4, 5, 6, 7};

int slotOf(const std::array<int, 3> &position) {
  return position[0] * 4 + position[1] * 2 + position[2];
}

const Cube &reference() {
  static const Cube solved;
  return solved;
}

// Rotulo de orientacao: eixo onde vive hoje o adesivo que nasceu em colors[1].
int labelOf(const Cube &cube, int piece) {
  const Color mark = reference().getCubies()[piece].colors[1];
  for (int axis = 0; axis < 3; ++axis) {
    if (ColorIsEqual(cube.getCubies()[piece].colors[axis], mark)) {
      return axis;
    }
  }
  return 1;
}

void applyMoveTo(Cube &cube, int move) {
  const int axis = move / 3;
  switch (move % 3) {
  case 0:
    cube.rotate(axis, 1, true);
    break;
  case 1:
    cube.rotate(axis, 1, false);
    break;
  default:
    cube.rotate(axis, 1, true);
    cube.rotate(axis, 1, true);
    break;
  }
}

int permToIndex(const std::array<int, 8> &pieceAt) {
  int index = 0;
  for (int slot = 1; slot <= MOVABLE; ++slot) {
    int smaller = 0;
    for (int other = slot + 1; other <= MOVABLE; ++other) {
      if (pieceAt[other] < pieceAt[slot]) {
        ++smaller;
      }
    }
    index = index * (8 - slot) + smaller;
  }
  return index;
}

std::array<int, 8> indexToPerm(int index) {
  std::array<int, 8> lehmer{};
  for (int slot = MOVABLE; slot >= 1; --slot) {
    lehmer[slot] = index % (8 - slot);
    index /= (8 - slot);
  }
  std::array<int, 8> available = {1, 2, 3, 4, 5, 6, 7, 0};
  int left = MOVABLE;
  std::array<int, 8> pieceAt{};
  for (int slot = 1; slot <= MOVABLE; ++slot) {
    const int pick = lehmer[slot];
    pieceAt[slot] = available[pick];
    for (int i = pick; i < left - 1; ++i) {
      available[i] = available[i + 1];
    }
    --left;
  }
  return pieceAt;
}

int oriToIndex(const std::array<int, 8> &labelAt) {
  int index = 0;
  for (int slot = MOVABLE; slot >= 1; --slot) {
    index = index * 3 + labelAt[slot];
  }
  return index;
}

std::array<int, 8> indexToOri(int index) {
  std::array<int, 8> labelAt{};
  for (int slot = 1; slot <= MOVABLE; ++slot) {
    labelAt[slot] = index % 3;
    index /= 3;
  }
  return labelAt;
}

// Ranqueamento denso de k slots distintos entre os sete moveis.
template <std::size_t K>
int groupIndex(const std::array<int, K> &slots,
               const std::array<int, K> &labels) {
  std::array<int, MOVABLE> available = {1, 2, 3, 4, 5, 6, 7};
  int left = MOVABLE;
  int position = 0;
  for (std::size_t i = 0; i < K; ++i) {
    int rank = 0;
    while (available[rank] != slots[i]) {
      ++rank;
    }
    position = position * left + rank;
    for (int j = rank; j < left - 1; ++j) {
      available[j] = available[j + 1];
    }
    --left;
  }
  int orientation = 0;
  for (std::size_t i = K; i-- > 0;) {
    orientation = orientation * 3 + labels[i];
  }
  int size = 1;
  for (std::size_t i = 0; i < K; ++i) {
    size *= 3;
  }
  return position * size + orientation;
}

struct Tables {
  std::array<std::array<int, 8>, MOVE_COUNT> sigma{};
  std::array<std::array<std::array<int, 3>, 8>, MOVE_COUNT> label{};
  std::vector<uint16_t> permMove;
  std::vector<uint16_t> oriMove;
  std::vector<uint8_t> pdbPerm;
  std::vector<uint8_t> pdbOri;
  std::vector<uint8_t> pdbGroupA;
  std::vector<uint8_t> pdbGroupB;
  bool ready = false;
};

Tables &tables() {
  static Tables instance;
  return instance;
}

// Geometria do giro lida do Cube real: destino de cada slot e transposicao
// dos rotulos. Nenhum dos dois depende de qual peca ocupa o slot, e e isso
// que torna as projecoes das PDBs homomorficas.
void buildGeometry(Tables &t) {
  for (int move = 0; move < MOVE_COUNT; ++move) {
    const int axis = move / 3;
    const int first = (axis + 1) % 3;
    const int second = (axis + 2) % 3;

    Cube cube;
    applyMoveTo(cube, move);
    for (int piece = 0; piece < 8; ++piece) {
      t.sigma[move][piece] = slotOf(cube.getCubies()[piece].position);
    }

    for (int slot = 0; slot < 8; ++slot) {
      const bool inLayer = ((slot >> (2 - axis)) & 1) == 1;
      for (int value = 0; value < 3; ++value) {
        int turned = value;
        if (inLayer && move % 3 != 2) {
          if (value == first) {
            turned = second;
          } else if (value == second) {
            turned = first;
          }
        }
        t.label[move][slot][value] = turned;
      }
    }
  }
}

void buildMoveTables(Tables &t) {
  t.permMove.resize(static_cast<std::size_t>(PERM_COUNT) * MOVE_COUNT);
  for (int index = 0; index < PERM_COUNT; ++index) {
    const std::array<int, 8> before = indexToPerm(index);
    for (int move = 0; move < MOVE_COUNT; ++move) {
      std::array<int, 8> after{};
      for (int slot = 1; slot <= MOVABLE; ++slot) {
        after[t.sigma[move][slot]] = before[slot];
      }
      t.permMove[static_cast<std::size_t>(index) * MOVE_COUNT + move] =
          static_cast<uint16_t>(permToIndex(after));
    }
  }

  t.oriMove.resize(static_cast<std::size_t>(ORI_COUNT) * MOVE_COUNT);
  for (int index = 0; index < ORI_COUNT; ++index) {
    const std::array<int, 8> before = indexToOri(index);
    for (int move = 0; move < MOVE_COUNT; ++move) {
      std::array<int, 8> after{};
      for (int slot = 1; slot <= MOVABLE; ++slot) {
        after[t.sigma[move][slot]] = t.label[move][slot][before[slot]];
      }
      t.oriMove[static_cast<std::size_t>(index) * MOVE_COUNT + move] =
          static_cast<uint16_t>(oriToIndex(after));
    }
  }
}

// BFS a partir do objetivo. O conjunto dos nove movimentos e fechado por
// inversao, entao distancia a partir do objetivo e distancia ate ele.
std::vector<uint8_t> buildProjection(const std::vector<uint16_t> &moves,
                                     int size, int goal) {
  std::vector<uint8_t> distance(static_cast<std::size_t>(size), 255);
  distance[static_cast<std::size_t>(goal)] = 0;
  std::vector<int> frontier{goal};
  for (int depth = 0; !frontier.empty(); ++depth) {
    std::vector<int> next;
    for (int state : frontier) {
      for (int move = 0; move < MOVE_COUNT; ++move) {
        const int neighbour =
            moves[static_cast<std::size_t>(state) * MOVE_COUNT + move];
        if (distance[static_cast<std::size_t>(neighbour)] == 255) {
          distance[static_cast<std::size_t>(neighbour)] =
              static_cast<uint8_t>(depth + 1);
          next.push_back(neighbour);
        }
      }
    }
    frontier.swap(next);
  }
  return distance;
}

template <std::size_t K>
std::vector<uint8_t> buildGroup(const Tables &t,
                                const std::array<int, K> &pieces) {
  int positions = 1;
  int orientations = 1;
  for (std::size_t i = 0; i < K; ++i) {
    positions *= (MOVABLE - static_cast<int>(i));
    orientations *= 3;
  }
  std::vector<uint8_t> distance(
      static_cast<std::size_t>(positions) * orientations, 255);

  std::array<int, K> slots{};
  std::array<int, K> labels{};
  for (std::size_t i = 0; i < K; ++i) {
    slots[i] = pieces[i];
    labels[i] = 1;
  }
  const int goal = groupIndex<K>(slots, labels);
  distance[static_cast<std::size_t>(goal)] = 0;
  std::vector<int> frontier{goal};

  for (int depth = 0; !frontier.empty(); ++depth) {
    std::vector<int> next;
    for (int state : frontier) {
      // Decodifica pelo mesmo ranqueamento usado em groupIndex.
      std::array<int, K> currentSlots{};
      std::array<int, K> currentLabels{};
      {
        int rest = state / orientations;
        int orientation = state % orientations;
        for (std::size_t i = 0; i < K; ++i) {
          currentLabels[i] = orientation % 3;
          orientation /= 3;
        }
        std::array<int, K> ranks{};
        for (std::size_t i = K; i-- > 0;) {
          const int left = MOVABLE - static_cast<int>(i);
          ranks[i] = rest % left;
          rest /= left;
        }
        std::array<int, MOVABLE> available = {1, 2, 3, 4, 5, 6, 7};
        int left = MOVABLE;
        for (std::size_t i = 0; i < K; ++i) {
          currentSlots[i] = available[ranks[i]];
          for (int j = ranks[i]; j < left - 1; ++j) {
            available[j] = available[j + 1];
          }
          --left;
        }
      }

      for (int move = 0; move < MOVE_COUNT; ++move) {
        std::array<int, K> movedSlots{};
        std::array<int, K> movedLabels{};
        for (std::size_t i = 0; i < K; ++i) {
          movedSlots[i] = t.sigma[move][currentSlots[i]];
          movedLabels[i] = t.label[move][currentSlots[i]][currentLabels[i]];
        }
        const int neighbour = groupIndex<K>(movedSlots, movedLabels);
        if (distance[static_cast<std::size_t>(neighbour)] == 255) {
          distance[static_cast<std::size_t>(neighbour)] =
              static_cast<uint8_t>(depth + 1);
          next.push_back(neighbour);
        }
      }
    }
    frontier.swap(next);
  }
  return distance;
}

template <std::size_t K>
int groupLookup(const std::vector<uint8_t> &distance,
                const std::array<int, K> &pieces,
                const std::array<int, 8> &slotOfPiece,
                const std::array<int, 8> &labelAt) {
  std::array<int, K> slots{};
  std::array<int, K> labels{};
  for (std::size_t i = 0; i < K; ++i) {
    slots[i] = slotOfPiece[pieces[i]];
    labels[i] = labelAt[slots[i]];
  }
  return distance[static_cast<std::size_t>(groupIndex<K>(slots, labels))];
}

const Tables &ready() {
  build();
  return tables();
}

} // namespace

int State::index() const { return perm * ORI_COUNT + ori; }

bool State::solved() const {
  const State goal = solvedState();
  return perm == goal.perm && ori == goal.ori;
}

State solvedState() {
  std::array<int, 8> pieceAt{};
  std::array<int, 8> labelAt{};
  for (int slot = 1; slot <= MOVABLE; ++slot) {
    pieceAt[slot] = slot;
    labelAt[slot] = 1;
  }
  return State{permToIndex(pieceAt), oriToIndex(labelAt)};
}

State apply(State state, int move) {
  const Tables &t = ready();
  return State{
      t.permMove[static_cast<std::size_t>(state.perm) * MOVE_COUNT + move],
      t.oriMove[static_cast<std::size_t>(state.ori) * MOVE_COUNT + move]};
}

const char *moveName(int move) {
  static const char *const names[MOVE_COUNT] = {"R", "R'", "R2", "U", "U'",
                                                "U2", "F", "F'", "F2"};
  return names[move];
}

int moveAxis(int move) { return move / 3; }

bool moveIsClockwise(int move) { return move % 3 != 1; }

bool moveIsHalf(int move) { return move % 3 == 2; }

std::vector<Spin> homingSpins(const Cube &cube) {
  auto atHome = [](const Cube &c) {
    return slotOf(c.getCubies()[0].position) == 0 && labelOf(c, 0) == 1;
  };
  if (atHome(cube)) {
    return {};
  }

  struct Node {
    Cube cube;
    std::vector<Spin> path;
  };
  std::array<bool, 24> seen{};
  std::queue<Node> pending;
  pending.push(Node{cube, {}});
  seen[static_cast<std::size_t>(slotOf(cube.getCubies()[0].position) * 3 +
                                labelOf(cube, 0))] = true;

  while (!pending.empty()) {
    const Node node = pending.front();
    pending.pop();
    for (int axis = 0; axis < 3; ++axis) {
      for (int direction = 0; direction < 2; ++direction) {
        const Spin spin{axis, direction == 0};
        Cube turned = node.cube;
        turned.rotate(spin.axis, 1, spin.clockwise);
        turned.rotate(spin.axis, 0, !spin.clockwise);

        std::vector<Spin> path = node.path;
        path.push_back(spin);
        if (atHome(turned)) {
          return path;
        }
        const std::size_t key = static_cast<std::size_t>(
            slotOf(turned.getCubies()[0].position) * 3 + labelOf(turned, 0));
        if (!seen[key]) {
          seen[key] = true;
          pending.push(Node{turned, path});
        }
      }
    }
  }
  return {};
}

State stateOf(const Cube &cube) {
  std::array<int, 8> pieceAt{};
  std::array<int, 8> labelAt{};
  for (int piece = 1; piece < 8; ++piece) {
    const int slot = slotOf(cube.getCubies()[piece].position);
    pieceAt[slot] = piece;
    labelAt[slot] = labelOf(cube, piece);
  }
  return State{permToIndex(pieceAt), oriToIndex(labelAt)};
}

void build() {
  Tables &t = tables();
  if (t.ready) {
    return;
  }
  buildGeometry(t);
  buildMoveTables(t);
  const State goal = solvedState();
  t.pdbPerm = buildProjection(t.permMove, PERM_COUNT, goal.perm);
  t.pdbOri = buildProjection(t.oriMove, ORI_COUNT, goal.ori);
  t.pdbGroupA = buildGroup<3>(t, GROUP_A);
  t.pdbGroupB = buildGroup<4>(t, GROUP_B);
  t.ready = true;
}

// max das quatro tabelas. Admissivel e consistente porque cada uma e a
// distancia exata numa abstracao homomorfica e max preserva as duas
// propriedades. Ver docs/heuristica-a-estrela.md secao 5.
int value(State state) {
  const Tables &t = ready();
  const std::array<int, 8> pieceAt = indexToPerm(state.perm);
  const std::array<int, 8> labelAt = indexToOri(state.ori);
  std::array<int, 8> slotOfPiece{};
  for (int slot = 1; slot <= MOVABLE; ++slot) {
    slotOfPiece[pieceAt[slot]] = slot;
  }

  int best = t.pdbPerm[static_cast<std::size_t>(state.perm)];
  best = std::max<int>(best, t.pdbOri[static_cast<std::size_t>(state.ori)]);
  best = std::max(best, groupLookup<3>(t.pdbGroupA, GROUP_A, slotOfPiece, labelAt));
  best = std::max(best, groupLookup<4>(t.pdbGroupB, GROUP_B, slotOfPiece, labelAt));
  return best;
}

} // namespace heuristic
