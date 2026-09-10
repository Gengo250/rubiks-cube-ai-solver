#include "CubeSnapshot.hpp"

#include "Cube.hpp"

#include <array>
#include <cstddef>

namespace {

// Mesma ordem do "enum class Face"; o indice aqui vira o indice do snapshot.
constexpr std::array<Face, CubeSnapshot::FACE_COUNT> SNAPSHOT_FACES = {
    Face::Upper, Face::Lower, Face::Front,
    Face::Rear,  Face::Right, Face::Left,
};

}  // namespace

CubeSnapshot makeCubeSnapshot(const Cube& cube) {
  CubeSnapshot snapshot{};

  for (std::size_t face = 0; face < CubeSnapshot::FACE_COUNT; ++face) {
    for (std::size_t position = 0; position < CubeSnapshot::STICKERS_PER_FACE;
         ++position) {
      const Color color = cube.getColor(SNAPSHOT_FACES[face], position);

      snapshot.stickers[face * CubeSnapshot::STICKERS_PER_FACE + position] =
          static_cast<unsigned char>(color);
    }
  }

  return snapshot;
}
