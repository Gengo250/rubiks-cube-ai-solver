#include "MenuRenderer.hpp"

#include "MenuOptions.hpp"

#include <raylib.h>

#include <cstddef>

namespace {

// Paleta do menu. Troque estes valores para mudar o tema.
constexpr Color BACKGROUND_COLOR = Color{24, 26, 32, 255};
constexpr Color TITLE_COLOR = Color{240, 240, 245, 255};
constexpr Color HINT_COLOR = Color{130, 136, 150, 255};
constexpr Color ITEM_COLOR = Color{38, 41, 50, 255};
constexpr Color ITEM_SELECTED_COLOR = Color{58, 110, 165, 255};
constexpr Color ITEM_BORDER_COLOR = Color{70, 76, 90, 255};
constexpr Color TEXT_COLOR = Color{210, 214, 222, 255};
constexpr Color TEXT_SELECTED_COLOR = Color{255, 255, 255, 255};

// Retangulo da opcao de indice "position" na lista (0 = primeira linha).
Rectangle optionRect(std::size_t position) {
  const float y = static_cast<float>(
      MenuRenderer::LIST_TOP +
      static_cast<int>(position) *
          (MenuRenderer::ITEM_HEIGHT + MenuRenderer::ITEM_SPACING));

  return Rectangle{
      static_cast<float>(MenuRenderer::MARGIN),
      y,
      static_cast<float>(MenuRenderer::WINDOW_WIDTH - 2 * MenuRenderer::MARGIN),
      static_cast<float>(MenuRenderer::ITEM_HEIGHT),
  };
}

}  // namespace

MenuRenderer::MenuRenderer() : selected(0), choice(NO_CHOICE) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "MENU - CUBE 2X2X2");
  SetTargetFPS(TARGET_FPS);
}

MenuRenderer::~MenuRenderer() { CloseWindow(); }

bool MenuRenderer::shouldClose() const { return WindowShouldClose(); }

int MenuRenderer::drawFrame() {
  choice = NO_CHOICE;

  handleInput();

  BeginDrawing();
  draw();
  EndDrawing();

  return choice;
}

void MenuRenderer::handleInput() {
  const int count = static_cast<int>(MENU_OPTION_COUNT);

  // Teclado: setas ou W/S para navegar, ENTER/ESPACO para confirmar.
  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    selected = (selected + 1) % count;
  }
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    selected = (selected - 1 + count) % count;
  }
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
    choice = MENU_OPTIONS[static_cast<std::size_t>(selected)].index;
  }

  // Mouse: passar por cima destaca, clicar confirma.
  const Vector2 mouse = GetMousePosition();

  for (std::size_t i = 0; i < MENU_OPTION_COUNT; ++i) {
    if (!CheckCollisionPointRec(mouse, optionRect(i))) {
      continue;
    }

    selected = static_cast<int>(i);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      choice = MENU_OPTIONS[i].index;
    }
  }
}

void MenuRenderer::draw() const {
  ClearBackground(BACKGROUND_COLOR);

  DrawText("MENU - CUBE 2X2X2", MARGIN, TITLE_Y, TITLE_FONT_SIZE, TITLE_COLOR);
  DrawText("Setas/mouse para navegar, ENTER ou clique para confirmar. "
           "O cubo e impresso no terminal.",
           MARGIN, HINT_Y, HINT_FONT_SIZE, HINT_COLOR);

  for (std::size_t i = 0; i < MENU_OPTION_COUNT; ++i) {
    const Rectangle rect = optionRect(i);
    const bool isSelected = (static_cast<int>(i) == selected);

    DrawRectangleRec(rect, isSelected ? ITEM_SELECTED_COLOR : ITEM_COLOR);
    DrawRectangleLinesEx(rect, 1.0f, ITEM_BORDER_COLOR);

    const char* line =
        TextFormat("%d - %s", MENU_OPTIONS[i].index, MENU_OPTIONS[i].label);

    const int textX = static_cast<int>(rect.x) + ITEM_PADDING_X;
    const int textY =
        static_cast<int>(rect.y) + (ITEM_HEIGHT - ITEM_FONT_SIZE) / 2;

    DrawText(line, textX, textY, ITEM_FONT_SIZE,
             isSelected ? TEXT_SELECTED_COLOR : TEXT_COLOR);
  }
}
