#include "Menu.hpp"
#include "MenuRenderer.hpp"

int main (int argc, char *argv[]) {
  Menu menu;
  MenuRenderer renderer;

  while (!renderer.shouldClose()) {
    const int index = renderer.drawFrame();

    if (index == MenuRenderer::NO_CHOICE) {
      continue;
    }

    menu.inicializated_Menu(index);

    if (index == 0) {
      break;
    }
  }

  return 0;
}
