#pragma once

#include <cstddef>

class Cube;

// Estado do cubo em formato "burro": so numeros, sem tipos do projeto.
//
// Existe por causa de um conflito de nomes: o projeto tem "enum class Color"
// (Cube.hpp) e a raylib tem "struct Color". Os dois no mesmo arquivo nao
// compilam. Entao o painel grafico recebe este snapshot em vez do Cube.
struct CubeSnapshot {
  static constexpr std::size_t FACE_COUNT = 6;
  static constexpr std::size_t STICKERS_PER_FACE = 4;
  static constexpr std::size_t STICKER_COUNT = FACE_COUNT * STICKERS_PER_FACE;

  // Indice = face * STICKERS_PER_FACE + posicao.
  // A ordem das faces segue "enum class Face":
  //   0 Upper, 1 Lower, 2 Front, 3 Rear, 4 Right, 5 Left
  // O valor de cada adesivo segue "enum class Color":
  //   0 White, 1 Yellow, 2 Green, 3 Blue, 4 Red, 5 Orange
  unsigned char stickers[STICKER_COUNT];
};

// Le o cubo e devolve a copia em numeros.
CubeSnapshot makeCubeSnapshot(const Cube& cube);
