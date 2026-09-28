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
constexpr int STATUS_Y = NET_Y + 3 * FACE_STEP - 10;
constexpr int IDDFS_MAXIMUM_DEPTH = 8;
// Um movimento a cada doze quadros: rapido de assistir, lento de perder.
constexpr int FRAMES_PER_MOVE = 12;

constexpr Color BACKGROUND = {24, 26, 32, 255};
constexpr Color PANEL = {18, 20, 25, 255};
constexpr Color TITLE = {240, 240, 245, 255};
constexpr Color HINT = {130, 136, 150, 255};
constexpr Color ITEM = {38, 41, 50, 255};
constexpr Color SELECTED = {58, 110, 165, 255};
constexpr Color BORDER = {70, 76, 90, 255};
constexpr Color TEXT = {210, 214, 222, 255};
constexpr Color SELECTED_TEXT = {255, 255, 255, 255};
constexpr Color RESULT = {120, 200, 140, 255};
constexpr const char *FONT_FILE = "assets/fonts/DejaVuSans.ttf";

enum class Action {
  Quit,
  Back,
  OpenManual,
  OpenAi,
  Render,
  Shuffle,
  Rotate,
  SolveDepth,
  SolveBreadth,
  SolveAStar,
};

// A mesma tabela fornece os rotulos e os parametros dos movimentos.
struct MenuOption {
  const char *label;
  Action action;
  int axis = 0;
  int layer = 0;
  bool clockwise = true;
};

constexpr std::array<MenuOption, 3> HOME_OPTIONS = {{
    {"Finish", Action::Quit},
    {"Solve by Yourself", Action::OpenManual},
    {"Solve with AI", Action::OpenAi},
}};

constexpr std::array<MenuOption, 15> MANUAL_OPTIONS = {{
    {"Back", Action::Back},
    {"Render Cube", Action::Render},
    {"Shuffle Cube", Action::Shuffle},
    {"Move Front Clockwise", Action::Rotate, 2, 1, true},
    {"Move Left Clockwise", Action::Rotate, 0, 0, true},
    {"Move Front Counterclockwise", Action::Rotate, 2, 1, false},
    {"Move Right Clockwise", Action::Rotate, 0, 1, true},
    {"Move Right Counterclockwise", Action::Rotate, 0, 1, false},
    {"Move Left Counterclockwise", Action::Rotate, 0, 0, false},
    {"Move Top Clockwise", Action::Rotate, 1, 1, true},
    {"Move Top Counterclockwise", Action::Rotate, 1, 1, false},
    {"Move Lower Clockwise", Action::Rotate, 1, 0, true},
    {"Move Lower Counterclockwise", Action::Rotate, 1, 0, false},
    {"Move Rear Clockwise", Action::Rotate, 2, 0, true},
    {"Move Rear Counterclockwise", Action::Rotate, 2, 0, false},
}};

constexpr std::array<MenuOption, 5> AI_OPTIONS = {{
    {"Back", Action::Back},
    {"Shuffle Cube", Action::Shuffle},
    {"Depth-First Search", Action::SolveDepth},
    {"Breadth-First Search", Action::SolveBreadth},
    {"A* Search", Action::SolveAStar},
}};

struct OptionList {
  const MenuOption *items;
  int count;
};

OptionList optionsFor(MenuScreen screen) {
  if (screen == MenuScreen::Manual) {
    return {MANUAL_OPTIONS.data(), static_cast<int>(MANUAL_OPTIONS.size())};
  }
  if (screen == MenuScreen::Ai) {
    return {AI_OPTIONS.data(), static_cast<int>(AI_OPTIONS.size())};
  }
  return {HOME_OPTIONS.data(), static_cast<int>(HOME_OPTIONS.size())};
}

const char *titleFor(MenuScreen screen) {
  if (screen == MenuScreen::Manual) {
    return "VOCE RESOLVE";
  }
  if (screen == MenuScreen::Ai) {
    return "IA RESOLVE";
  }
  return "MENU - CUBE 2X2X2";
}

const char *hintFor(MenuScreen screen) {
  if (screen == MenuScreen::Manual) {
    return "Gire as faces ate resolver o cubo.";
  }
  if (screen == MenuScreen::Ai) {
    return "Embaralhe e escolha entre IDDFS e A*.";
  }
  return "Escolha quem resolve o cubo.";
}

std::string turnName(const Turn &turn) {
  static constexpr const char *FACES[3][2] = {
      {"L", "R"},
      {"D", "U"},
      {"B", "F"},
  };

  std::string name = FACES[turn.axis][turn.layer];
  if (turn.half) {
    name += '2';
  } else if (!turn.clockwise) {
    name += '\'';
  }
  return name;
}

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
  bool running = true;
  while (running && !WindowShouldClose()) {
    const int choice = readChoice();
    if (playing()) {
      advancePlayback();
    } else if (choice >= 0) {
      running = handle(choice);
    }

    BeginDrawing();
    draw();
    EndDrawing();
  }
}

int Menu::readChoice() {
  const int count = optionsFor(screen).count;
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

void Menu::openScreen(MenuScreen next) {
  screen = next;
  selected = 0;
  status.clear();
  activeSolver.clear();
  solution = Solution{};
  playback.clear();
  playbackIndex = 0;
  setupCount = 0;
  if (next == MenuScreen::Ai) {
    cubeVisible = true;
  }
}

bool Menu::handle(int choice) {
  const MenuOption &option = optionsFor(screen).items[choice];
  switch (option.action) {
  case Action::Quit:
    return false;
  case Action::Back:
    openScreen(MenuScreen::Home);
    break;
  case Action::OpenManual:
    openScreen(MenuScreen::Manual);
    break;
  case Action::OpenAi:
    openScreen(MenuScreen::Ai);
    break;
  case Action::Render:
    cubeVisible = true;
    break;
  case Action::Shuffle:
    cube.shuffle();
    solution = Solution{};
    activeSolver.clear();
    status = "Cubo embaralhado.";
    break;
  case Action::Rotate:
    cube.rotate(option.axis, option.layer, option.clockwise);
    break;
  case Action::SolveDepth:
    startDepthSolve();
    break;
  case Action::SolveBreadth:
    status = "Busca em largura nao implementada. Use a busca A*.";
    break;
  case Action::SolveAStar:
    startSolve();
    break;
  }
  return true;
}

void Menu::startDepthSolve() {
  cubeVisible = true;
  const double started = GetTime();
  const DepthSearchResult result =
      DepthFirstSearch::iterativeDeepeningSearch(cube, IDDFS_MAXIMUM_DEPTH);

  solution = Solution{};
  solution.solved = result.solved;
  solution.expanded = static_cast<long long>(result.visitedStates);
  solution.milliseconds = (GetTime() - started) * 1000.0;

  for (const DepthMove &move : result.solution) {
    const Turn turn{move.axis, move.layer, move.clockwise, false};
    solution.turns.push_back(turn);

    if (!solution.notation.empty()) {
      solution.notation += ' ';
    }
    solution.notation += turnName(turn);
  }

  playback = solution.turns;
  playbackIndex = 0;
  setupCount = 0;
  frames = 0;
  activeSolver = "IDDFS";

  if (!solution.solved) {
    status = "IDDFS nao encontrou solucao ate a profundidade " +
             std::to_string(IDDFS_MAXIMUM_DEPTH) + ".";
  } else if (solution.turns.empty()) {
    status = "O cubo ja esta resolvido.";
  } else {
    status = "IDDFS encontrou a solucao. Aplicando os movimentos.";
  }
}

void Menu::startSolve() {
  cubeVisible = true;
  solution = solveAStar(cube);
  playback = solution.setup;
  playback.insert(playback.end(), solution.turns.begin(), solution.turns.end());
  setupCount = solution.setup.size();
  playbackIndex = 0;
  frames = 0;
  activeSolver = "A*";

  if (!solution.solved) {
    status = "A busca nao encontrou solucao.";
  } else if (solution.turns.empty()) {
    status = "O cubo ja esta resolvido.";
  } else {
    status = "Solucao encontrada. Aplicando os movimentos.";
  }
}

bool Menu::playing() const { return playbackIndex < playback.size(); }

void Menu::advancePlayback() {
  if (++frames < FRAMES_PER_MOVE) {
    return;
  }
  frames = 0;

  const Turn &turn = playback[playbackIndex];
  cube.rotate(turn.axis, turn.layer, turn.clockwise);
  if (turn.half) {
    cube.rotate(turn.axis, turn.layer, turn.clockwise);
  }
  ++playbackIndex;

  if (!playing() && solution.solved) {
    status = "Cubo resolvido pela busca " + activeSolver + ".";
  }
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

  drawText(titleFor(screen), MARGIN, 22, 28, TITLE);
  drawText(hintFor(screen), MARGIN, 58, 14, HINT);

  const OptionList options = optionsFor(screen);
  for (int i = 0; i < options.count; ++i) {
    const Rectangle rect = optionRect(i);
    const bool active = i == selected;
    DrawRectangleRec(rect, active ? SELECTED : ITEM);
    DrawRectangleLinesEx(rect, 1.0f, BORDER);
    drawText(TextFormat("%d - %s", i, options.items[i].label),
             static_cast<int>(rect.x) + 14, static_cast<int>(rect.y) + 8, 18,
             active ? SELECTED_TEXT : TEXT);
  }

  drawText("ESTADO DO CUBO", MENU_WIDTH + MARGIN, 22, 28, TITLE);
  drawText("Planificacao 2x2x2", MENU_WIDTH + MARGIN, 58, 14, HINT);
  if (cubeVisible) {
    drawCube();
  } else {
    drawText("Escolha \"1 - Render Cube\" para desenhar o cubo aqui.",
             MENU_WIDTH + MARGIN, WINDOW_HEIGHT / 2, 18, HINT);
  }
  drawStatus();
}

void Menu::drawStatus() const {
  int y = STATUS_Y;
  if (!status.empty()) {
    drawText(status.c_str(), MENU_WIDTH + MARGIN, y, 18,
             playing() ? TEXT : RESULT);
    y += 26;
  }

  if (!solution.solved) {
    return;
  }

  drawText(TextFormat("%d movimentos HTM   %lld estados visitados   %.1f ms",
                      static_cast<int>(solution.turns.size()),
                      solution.expanded, solution.milliseconds),
           MENU_WIDTH + MARGIN, y, 16, HINT);
  y += 24;

  if (!solution.notation.empty()) {
    drawText(solution.notation.c_str(), MENU_WIDTH + MARGIN, y, 20, TEXT);
    y += 26;
  }

  if (playing()) {
    if (playbackIndex < setupCount) {
      drawText("Reorientando o cubo inteiro: a peca fixa volta para casa.",
               MENU_WIDTH + MARGIN, y, 16, HINT);
    } else {
      const std::size_t step = playbackIndex - setupCount;
      drawText(TextFormat("Movimento %d de %d: %s",
                          static_cast<int>(step + 1),
                          static_cast<int>(solution.turns.size()),
                          turnName(playback[playbackIndex]).c_str()),
               MENU_WIDTH + MARGIN, y, 16, HINT);
    }
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