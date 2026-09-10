#include "CubeSnapshot.hpp"
#include "Menu.hpp"
#include "MenuRenderer.hpp"

int main (int argc, char *argv[]) {
  Menu menu;
  MenuRenderer renderer;

  // Copia do estado do cubo desenhada no painel direito.
  // So precisa ser refeita depois de um movimento.
  CubeSnapshot snapshot = makeCubeSnapshot(menu.getCube());

  while (!renderer.shouldClose()) {
    const int index = renderer.drawFrame(snapshot);

    if (index == MenuRenderer::NO_CHOICE) {
      continue;
    }

    menu.inicializated_Menu(index);
    snapshot = makeCubeSnapshot(menu.getCube());

    if (index == 0) {
      break;
    }
  }

  return 0;
}
