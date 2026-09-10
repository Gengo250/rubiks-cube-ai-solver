#include "MenuRenderer.hpp"

#include "CubeSnapshot.hpp"
#include "MenuOptions.hpp"

#include <raylib.h>

#include <array>
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

// Painel direito: fundo levemente diferente para separar do menu.
constexpr Color PANEL_BACKGROUND_COLOR = Color{18, 20, 25, 255};
constexpr Color STICKER_BORDER_COLOR = Color{18, 20, 25, 255};

// Cores dos adesivos, na ordem de "enum class Color":
// White, Yellow, Green, Blue, Red, Orange.
constexpr std::array<Color, 6> STICKER_COLORS = {
    Color{240, 240, 240, 255},  // White
    Color{235, 200, 40, 255},   // Yellow
    Color{40, 170, 90, 255},    // Green
    Color{45, 105, 200, 255},   // Blue
    Color{200, 55, 55, 255},    // Red
    Color{230, 130, 40, 255},   // Orange
};

// --- Fonte do menu: mexa aqui para trocar a letra ---
// Caminho relativo a pasta do executavel; o CMake copia "assets/" para la.
constexpr const char* MENU_FONT_FILE = "assets/fonts/DejaVuSans.ttf";

// Tamanho em que o atlas de glifos e gerado. Mantenha >= ao maior
// *_FONT_SIZE do header, senao a raylib amplia o glifo e o texto borra.
constexpr int MENU_FONT_ATLAS_SIZE = 64;

// Espaco extra entre caracteres, em pixels.
constexpr float MENU_FONT_SPACING = 1.0f;

// Carregada no construtor (precisa da janela aberta) e liberada no destrutor.
// Se o .ttf nao for encontrado, a raylib devolve a fonte embutida e o menu
// continua funcionando normalmente.
Font menuFont = Font{};

// true quando menuFont e a fonte embutida da raylib, e nao o .ttf.
bool usingDefaultFont() {
  return menuFont.texture.id == GetFontDefault().texture.id;
}

// Procura o .ttf ao lado do executavel; se nao achar, tenta o caminho
// relativo ao diretorio atual (util ao rodar a partir da raiz do projeto).
const char* menuFontPath() {
  const char* nextToBinary =
      TextFormat("%s%s", GetApplicationDirectory(), MENU_FONT_FILE);

  return FileExists(nextToBinary) ? nextToBinary : MENU_FONT_FILE;
}

// Desenha texto do menu com a fonte carregada.
void drawMenuText(const char* text, int x, int y, int fontSize, Color color) {
  DrawTextEx(menuFont, text,
             Vector2{static_cast<float>(x), static_cast<float>(y)},
             static_cast<float>(fontSize), MENU_FONT_SPACING, color);
}

// Retangulo da opcao de indice "position" na lista (0 = primeira linha).
// A lista ocupa so o painel esquerdo.
Rectangle optionRect(std::size_t position) {
  const float y = static_cast<float>(
      MenuRenderer::LIST_TOP +
      static_cast<int>(position) *
          (MenuRenderer::ITEM_HEIGHT + MenuRenderer::ITEM_SPACING));

  return Rectangle{
      static_cast<float>(MenuRenderer::MARGIN),
      y,
      static_cast<float>(MenuRenderer::MENU_PANEL_WIDTH -
                         2 * MenuRenderer::MARGIN),
      static_cast<float>(MenuRenderer::ITEM_HEIGHT),
  };
}

// --- Geometria da planificacao ---------------------------------------------
// Cada face e um bloco 2x2 de adesivos; as seis faces formam a cruz:
//
//         U
//     L   F   R   B
//         D

constexpr int FACE_BLOCK =
    2 * MenuRenderer::STICKER_SIZE + MenuRenderer::STICKER_GAP;

constexpr int NET_WIDTH = 4 * FACE_BLOCK + 3 * MenuRenderer::FACE_GAP;
constexpr int NET_HEIGHT = 3 * FACE_BLOCK + 2 * MenuRenderer::FACE_GAP;

// Centralizada dentro do painel direito.
constexpr int NET_X = MenuRenderer::VIEW_PANEL_X +
                      (MenuRenderer::VIEW_PANEL_WIDTH - NET_WIDTH) / 2;
constexpr int NET_Y = (MenuRenderer::WINDOW_HEIGHT - NET_HEIGHT) / 2;

// Uma face na planificacao: qual face do snapshot, onde fica na cruz e o rotulo.
struct NetFace {
  std::size_t faceIndex;  // ordem de "enum class Face"
  int column;             // 0..3 na grade da cruz
  int row;                // 0..2 na grade da cruz
  const char* label;
};

// A ordem das faces do snapshot e: 0 Upper, 1 Lower, 2 Front, 3 Rear,
// 4 Right, 5 Left. As laterais ficam na mesma sequencia impressa no terminal.
constexpr std::array<NetFace, CubeSnapshot::FACE_COUNT> NET_FACES = {{
    {0, 1, 0, "U - Upper"},
    {5, 0, 1, "L - Left"},
    {2, 1, 1, "F - Front"},
    {4, 2, 1, "R - Right"},
    {3, 3, 1, "B - Rear"},
    {1, 1, 2, "D - Lower"},
}};

// Canto superior esquerdo do bloco de uma face na cruz.
Vector2 faceOrigin(const NetFace& face) {
  return Vector2{
      static_cast<float>(NET_X +
                         face.column * (FACE_BLOCK + MenuRenderer::FACE_GAP)),
      static_cast<float>(NET_Y +
                         face.row * (FACE_BLOCK + MenuRenderer::FACE_GAP)),
  };
}

// Desenha os 4 adesivos de uma face mais o rotulo acima dela.
void drawNetFace(const NetFace& face, const CubeSnapshot& cube) {
  const Vector2 origin = faceOrigin(face);

  drawMenuText(face.label, static_cast<int>(origin.x),
               static_cast<int>(origin.y) - MenuRenderer::FACE_LABEL_FONT_SIZE -
                   6,
               MenuRenderer::FACE_LABEL_FONT_SIZE, HINT_COLOR);

  for (std::size_t position = 0; position < CubeSnapshot::STICKERS_PER_FACE;
       ++position) {
    const std::size_t row = position / 2;
    const std::size_t column = position % 2;

    const Rectangle sticker{
        origin.x + static_cast<float>(column) *
                       (MenuRenderer::STICKER_SIZE + MenuRenderer::STICKER_GAP),
        origin.y + static_cast<float>(row) *
                       (MenuRenderer::STICKER_SIZE + MenuRenderer::STICKER_GAP),
        static_cast<float>(MenuRenderer::STICKER_SIZE),
        static_cast<float>(MenuRenderer::STICKER_SIZE),
    };

    const unsigned char value =
        cube.stickers[face.faceIndex * CubeSnapshot::STICKERS_PER_FACE +
                      position];

    const Color color = (value < STICKER_COLORS.size())
                            ? STICKER_COLORS[value]
                            : ITEM_BORDER_COLOR;

    DrawRectangleRec(sticker, color);
    DrawRectangleLinesEx(sticker, 2.0f, STICKER_BORDER_COLOR);
  }
}

}  // namespace

MenuRenderer::MenuRenderer() : selected(0), choice(NO_CHOICE) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "MENU - CUBE 2X2X2");
  SetTargetFPS(TARGET_FPS);

  // Depois de InitWindow: LoadFontEx precisa do contexto grafico ativo.
  menuFont = LoadFontEx(menuFontPath(), MENU_FONT_ATLAS_SIZE, nullptr, 0);

  if (usingDefaultFont()) {
    TraceLog(LOG_WARNING, "MENU: '%s' nao encontrado, usando a fonte padrao",
             MENU_FONT_FILE);
  } else {
    // Suaviza o glifo quando o atlas e reduzido para o tamanho de tela.
    SetTextureFilter(menuFont.texture, TEXTURE_FILTER_BILINEAR);
  }
}

MenuRenderer::~MenuRenderer() {
  // Liberar antes de CloseWindow: a textura do atlas vive no contexto grafico.
  if (!usingDefaultFont()) {
    UnloadFont(menuFont);
  }

  menuFont = Font{};

  CloseWindow();
}

bool MenuRenderer::shouldClose() const { return WindowShouldClose(); }

int MenuRenderer::drawFrame(const CubeSnapshot& cube) {
  choice = NO_CHOICE;

  handleInput();

  BeginDrawing();
  draw(cube);
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

void MenuRenderer::draw(const CubeSnapshot& cube) const {
  ClearBackground(BACKGROUND_COLOR);

  drawMenuPanel();
  drawCubePanel(cube);

  // Divisoria entre os dois paineis.
  DrawLine(VIEW_PANEL_X, 0, VIEW_PANEL_X, WINDOW_HEIGHT, ITEM_BORDER_COLOR);
}

void MenuRenderer::drawMenuPanel() const {
  drawMenuText("MENU - CUBE 2X2X2", MARGIN, TITLE_Y, TITLE_FONT_SIZE,
               TITLE_COLOR);
  drawMenuText("Setas/mouse para navegar, ENTER ou clique para confirmar.",
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

    drawMenuText(line, textX, textY, ITEM_FONT_SIZE,
                 isSelected ? TEXT_SELECTED_COLOR : TEXT_COLOR);
  }
}

void MenuRenderer::drawCubePanel(const CubeSnapshot& cube) const {
  // Scissor mode: nada desenhado aqui dentro vaza para o painel do menu.
  BeginScissorMode(VIEW_PANEL_X, 0, VIEW_PANEL_WIDTH, WINDOW_HEIGHT);

  DrawRectangle(VIEW_PANEL_X, 0, VIEW_PANEL_WIDTH, WINDOW_HEIGHT,
                PANEL_BACKGROUND_COLOR);

  drawMenuText("ESTADO DO CUBO", VIEW_PANEL_X + MARGIN, TITLE_Y,
               TITLE_FONT_SIZE, TITLE_COLOR);
  drawMenuText("Planificacao 2x2x2: a cruz segue a mesma ordem impressa no "
               "terminal.",
               VIEW_PANEL_X + MARGIN, HINT_Y, HINT_FONT_SIZE, HINT_COLOR);

  for (const NetFace& face : NET_FACES) {
    drawNetFace(face, cube);
  }

  EndScissorMode();
}
