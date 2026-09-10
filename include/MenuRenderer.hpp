#pragma once

#include "CubeSnapshot.hpp"

// IMPORTANTE: este header NAO inclui <raylib.h>.
// O projeto tem "enum class Color" em Cube.hpp e a raylib tem "struct Color".
// Manter a raylib apenas dentro de MenuRenderer.cpp evita esse conflito de nomes.
// Pelo mesmo motivo o painel do cubo recebe um CubeSnapshot, e nao um Cube.

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

  // Processa a entrada e desenha um frame: menu a esquerda, cubo a direita.
  // Retorna o indice da opcao escolhida neste frame, ou NO_CHOICE.
  int drawFrame(const CubeSnapshot& cube);

  // Liga o desenho do cubo no painel direito.
  // Chamado quando o usuario escolhe "Render Cube": o cubo passa a ser
  // desenhado na janela, e nao impresso no terminal.
  void showCube();

  // --- Constantes de layout: mexa aqui para personalizar o visual ---
  static constexpr int WINDOW_WIDTH = 1810;
  static constexpr int WINDOW_HEIGHT = 1000;
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

  // --- Painel esquerdo (menu) e painel direito (cubo) ---
  // A janela e uma so: a raylib nao abre duas janelas no mesmo processo.
  // O "segundo painel" e esta faixa a direita, recortada com scissor mode.
  static constexpr int MENU_PANEL_WIDTH = 620;
  static constexpr int VIEW_PANEL_X = MENU_PANEL_WIDTH;
  static constexpr int VIEW_PANEL_WIDTH = WINDOW_WIDTH - MENU_PANEL_WIDTH;

  // Planificacao do cubo: cada face e um bloco 2x2 de adesivos.
  static constexpr int STICKER_SIZE = 90;
  static constexpr int STICKER_GAP = 4;
  static constexpr int FACE_GAP = 34;
  static constexpr int FACE_LABEL_FONT_SIZE = 18;

 private:
  void handleInput();
  void draw(const CubeSnapshot& cube) const;
  void drawMenuPanel() const;
  void drawCubePanel(const CubeSnapshot& cube) const;

  int selected;     // opcao destacada (navegacao por teclado/mouse)
  int choice;       // opcao confirmada neste frame
  bool cubeVisible; // true depois que "Render Cube" foi escolhido
};
