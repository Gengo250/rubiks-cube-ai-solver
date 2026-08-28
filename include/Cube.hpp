#pragma once

#include <array>

enum class Color { 
  White, 
  Yellow, 
  Green, 
  Blue, 
  Red, 
  Orange 
};

class Cube {
 public:

 private:
  static constexpr int FACE_SIZE = 2;
  static constexpr int FACE_COUNT = 6;
  static constexpr int STICKERS_PER_FACE = 4;

  std::array<std::array<Color, STICKERS_PER_FACE>, FACE_COUNT> faces;
};
