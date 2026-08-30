#include "Cube.hpp"
#include "CubeRenderer.hpp"
#include <cassert>

int main (int argc, char *argv[]) {
  Cube cube; 
  CubeRenderer::render(cube);
  

  return 0;
}
