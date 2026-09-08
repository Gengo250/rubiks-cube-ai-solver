#pragma once

// IMPORTANTE: este header NAO inclui <raylib.h>.
// O projeto tem "enum class Color" em Cube.hpp e a raylib tem "struct Color".
// Manter a raylib apenas dentro de MenuRenderer.cpp evita esse conflito de nomes.

class MenuRenderer {
 public:
  // Valor devolvido por drawFrame() quando o usuario ainda nao escolheu nada.
  static constexpr int NO_CHOICE = -1;

  // Abre a janela.
  MenuRenderer();
  // Fecha a janela.
  ~MenuRenderer();

  // true quando o usuario fechou a janela (botao X ou ESC).
  bool shouldClose() const;

  // Processa a entrada e desenha um frame.
  // Retorna o indice da opcao escolhida neste frame, ou NO_CHOICE.
  int drawFrame();

  // --- Constantes de layout: mexa aqui para personalizar o visual ---
  static constexpr int WINDOW_WIDTH = 560;
  static constexpr int WINDOW_HEIGHT = 690;
  static constexpr int TARGET_FPS = 60;

  static constexpr int MARGIN = 24;
  static constexpr int TITLE_Y = 22;
  static constexpr int TITLE_FONT_SIZE = 28;
  static constexpr int HINT_Y = 58;
  static constexpr int HINT_FONT_SIZE = 14;

  static constexpr int LIST_TOP = 96;
  static constexpr int ITEM_HEIGHT = 34;
  static constexpr int ITEM_SPACING = 3;
  static constexpr int ITEM_FONT_SIZE = 18;
  static constexpr int ITEM_PADDING_X = 14;

 private:
  void handleInput();
  void draw() const;

  int selected;  // opcao destacada (navegacao por teclado/mouse)
  int choice;    // opcao confirmada neste frame
};
