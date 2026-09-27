#pragma once

#include "Cube.hpp"
#include "Solver.hpp"

#include <string>
#include <vector>

// Telas do menu: escolha inicial, jogo manual e busca por IA.
enum class MenuScreen { Home, Manual, Ai };

class Menu {
public:
  Menu();
  ~Menu();

  Menu(const Menu &) = delete;
  Menu &operator=(const Menu &) = delete;
  void run();

private:
  int readChoice();
  bool handle(int choice);
  void openScreen(MenuScreen next);
  void startSolve();
  bool playing() const;
  void advancePlayback();
  void draw() const;
  void drawCube() const;
  void drawStatus() const;
  void drawText(const char *text, int x, int y, int size, Color color) const;

  Cube cube;
  Font font{};
  MenuScreen screen = MenuScreen::Home;
  int selected = 0;
  bool cubeVisible = false;
  std::string status;

  Solution solution;
  // Reorientacao e solucao na ordem em que sao aplicadas ao cubo.
  std::vector<Turn> playback;
  std::size_t playbackIndex = 0;
  std::size_t setupCount = 0;
  int frames = 0;
};
