#pragma once

#include "Cube.hpp"


class CubeRenderer{
  public:
    static void render(const Cube& cube);

  private:
    static char colorToChar(Color color);
};

