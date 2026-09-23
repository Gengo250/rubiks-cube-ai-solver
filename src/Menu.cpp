#include "Menu.hpp"

#include <array>

namespace {

constexpr int WINDOW_WIDTH = 1810;
constexpr int WINDOW_HEIGHT = 1000;
constexpr int MENU_WIDTH = 620;
constexpr int MARGIN = 24;
constexpr int LIST_TOP = 96;
constexpr int ITEM_HEIGHT = 34;
constexpr int ITEM_SPACING = 3;
constexpr int STICKER_SIZE = 90;
constexpr int STICKER_STEP = STICKER_SIZE + 4;
constexpr int FACE_SIZE = 2 * STICKER_SIZE + 4;
constexpr int FACE_STEP = FACE_SIZE + 34;
constexpr int NET_X =
    MENU_WIDTH + (WINDOW_WIDTH - MENU_WIDTH - (4 * FACE_STEP - 34)) / 2;
constexpr int NET_Y = (WINDOW_HEIGHT - (3 * FACE_STEP - 34)) / 2;

constexpr Color BACKGROUND = {24, 26, 32, 255};
constexpr Color PANEL = {18, 20, 25, 255};
constexpr Color TITLE = {240, 240, 245, 255};
constexpr Color HINT = {130, 136, 150, 255};
constexpr Color ITEM = {38, 41, 50, 255};
constexpr Color SELECTED = {58, 110, 165, 255};
constexpr Color BORDER = {70, 76, 90, 255};
constexpr Color TEXT = {210, 214, 222, 255};
constexpr Color SELECTED_TEXT = {255, 255, 255, 255};
constexpr const char *FONT_FILE = "assets/fonts/DejaVuSans.ttf";

// A mesma tabela fornece os rotulos e os parametros dos movimentos.
struct MenuOption {
  const char *label;
  int axis = -1;
  int layer = 0;
  bool clockwise = true;
};

constexpr std::array<MenuOption, 17> OPTIONS = {{
    {"Finish"},
    {"Render Cube"},
    {"Shuffle Cube"},
    {"Move Front Clockwise", 2, 1, true},
    {"Move Left Clockwise", 0, 0, true},
    {"Move Front Counterclockwise", 2, 1, false},
    {"Move Right Clockwise", 0, 1, true},
    {"Move Right Counterclockwise", 0, 1, false},
    {"Move Left Counterclockwise", 0, 0, false},
    {"Move Top Clockwise", 1, 1, true},
    {"Move Top Counterclockwise", 1, 1, false},
    {"Move Lower Clockwise", 1, 0, true},
    {"Move Lower Counterclockwise", 1, 0, false},
    {"Move Rear Clockwise", 2, 0, true},
    {"Move Rear Counterclockwise", 2, 0, false},
    {"Reset Cube"},
    {"Check if Cube is Solved"},
}};

struct NetFace {
  int axis;
  int layer;
  int column;
  int row;
  const char *label;
};

constexpr std::array<NetFace, 6> NET_FACES = {{
    {1, 1, 1, 0, "U - Upper"},
    {0, 0, 0, 1, "L - Left"},
    {2, 1, 1, 1, "F - Front"},
    {0, 1, 2, 1, "R - Right"},
    {2, 0, 3, 1, "B - Rear"},
    {1, 0, 1, 2, "D - Lower"},
}};

Rectangle optionRect(int index) {
  return {static_cast<float>(MARGIN),
          static_cast<float>(LIST_TOP + index * (ITEM_HEIGHT + ITEM_SPACING)),
          static_cast<float>(MENU_WIDTH - 2 * MARGIN),
          static_cast<float>(ITEM_HEIGHT)};
}

} // namespace

Menu::Menu() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "MENU - CUBE 2X2X2");
  SetTargetFPS(60);

  const char *path = TextFormat("%s%s", GetApplicationDirectory(), FONT_FILE);
  font = LoadFontEx(FileExists(path) ? path : FONT_FILE, 64, nullptr, 0);
  if (font.texture.id != GetFontDefault().texture.id) {
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
  }
}

Menu::~Menu() {
  if (font.texture.id != GetFontDefault().texture.id) {
    UnloadFont(font);
  }
  CloseWindow();
}

void Menu::run() {
  while (!WindowShouldClose()) {
    const int choice = readChoice();
    if (choice == 0) {
      break;
    }
    if (choice == 1) {
      cubeVisible = true;
    } else if (choice == 2) {
      cube.shuffle();
      statusMessage = "";
    } else if (choice >= 3 && choice <= 14) {
      const MenuOption &option = OPTIONS[choice];
      cube.rotate(option.axis, option.layer, option.clockwise);
      statusMessage = "";
    } else if (choice == 15) {
      cube.reset();
      cubeVisible = true;
      statusMessage = "Cube reset successfully.";
    } else if (choice == 16) {
      statusMessage =
          cube.isSolved() ? "The cube is solved!" : "The cube is not solved.";
    }

    BeginDrawing();
    draw();
    EndDrawing();
  }
}

int Menu::readChoice() {
  constexpr int count = static_cast<int>(OPTIONS.size());
  int choice = -1;

  if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
    selected = (selected + 1) % count;
  }
  if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
    selected = (selected - 1 + count) % count;
  }
  if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
    choice = selected;
  }

  const Vector2 mouse = GetMousePosition();
  const Vector2 delta = GetMouseDelta();
  const bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
  // Um mouse parado nao deve desfazer a selecao feita pelo teclado.
  if (delta.x != 0 || delta.y != 0 || clicked) {
    for (int i = 0; i < count; ++i) {
      if (CheckCollisionPointRec(mouse, optionRect(i))) {
        selected = i;
        if (clicked) {
          choice = i;
        }
        break;
      }
    }
  }
  return choice;
}

void Menu::drawText(const char *text, int x, int y, int size,
                    Color color) const {
  DrawTextEx(font, text, {static_cast<float>(x), static_cast<float>(y)},
             static_cast<float>(size), 1.0f, color);
}

void Menu::draw() const {
  ClearBackground(BACKGROUND);
  DrawRectangle(MENU_WIDTH, 0, WINDOW_WIDTH - MENU_WIDTH, WINDOW_HEIGHT, PANEL);
  DrawLine(MENU_WIDTH, 0, MENU_WIDTH, WINDOW_HEIGHT, BORDER);

  drawText("MENU - CUBE 2X2X2", MARGIN, 22, 28, TITLE);
  drawText("Setas/mouse para navegar, ENTER ou clique para confirmar.", MARGIN,
           58, 14, HINT);

  for (int i = 0; i < static_cast<int>(OPTIONS.size()); ++i) {
    const Rectangle rect = optionRect(i);
    const bool active = i == selected;
    DrawRectangleRec(rect, active ? SELECTED : ITEM);
    DrawRectangleLinesEx(rect, 1.0f, BORDER);
    drawText(TextFormat("%d - %s", i, OPTIONS[i].label),
             static_cast<int>(rect.x) + 14, static_cast<int>(rect.y) + 8, 18,
             active ? SELECTED_TEXT : TEXT);
  }

  drawText("ESTADO DO CUBO", MENU_WIDTH + MARGIN, 22, 28, TITLE);
  drawText("Planificacao 2x2x2", MENU_WIDTH + MARGIN, 58, 14, HINT);

  if (statusMessage[0] != '\0') {
    drawText(statusMessage, MENU_WIDTH + MARGIN, 88, 18, SELECTED_TEXT);
  }

  if (cubeVisible) {
    drawCube();
  } else {
    drawText("Escolha \"1 - Render Cube\" para desenhar o cubo aqui.",
             MENU_WIDTH + MARGIN, WINDOW_HEIGHT / 2, 18, HINT);
  }
}

void Menu::drawCube() const {
  for (const NetFace &face : NET_FACES) {
    const int x = NET_X + face.column * FACE_STEP;
    const int y = NET_Y + face.row * FACE_STEP;
    drawText(face.label, x, y - 24, 18, HINT);

    for (const Cubie &cubie : cube.getCubies()) {
      if (cubie.position[face.axis] != face.layer) {
        continue;
      }

      const auto &p = cubie.position;
      int row;
      int column;
      // Projecao da peca na cruz, sempre olhando cada face de fora.
      if (face.axis == 0) {
        row = 1 - p[1];
        column = face.layer == 1 ? 1 - p[2] : p[2];
      } else if (face.axis == 1) {
        row = face.layer == 1 ? p[2] : 1 - p[2];
        column = p[0];
      } else {
        row = 1 - p[1];
        column = face.layer == 1 ? p[0] : 1 - p[0];
      }

      const Rectangle sticker{static_cast<float>(x + column * STICKER_STEP),
                              static_cast<float>(y + row * STICKER_STEP),
                              STICKER_SIZE, STICKER_SIZE};
      DrawRectangleRec(sticker, cubie.colors[face.axis]);
      DrawRectangleLinesEx(sticker, 2.0f, PANEL);
    }
  }
}
