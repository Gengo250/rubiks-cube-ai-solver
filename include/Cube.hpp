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
enum class Face{
  Upper,
  Lower,
  Front,
  Rear, 
  Right,
  Left,
};
class Cube {
 public:

  Cube();

 private:
  static constexpr int FACE_SIZE = 2;
  static constexpr int FACE_COUNT = 6;
  static constexpr int STICKERS_PER_FACE = FACE_SIZE * FACE_SIZE;

  std::array<std::array<Color, STICKERS_PER_FACE>, FACE_COUNT> faces;
};
