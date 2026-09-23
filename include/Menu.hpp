#pragma once
#include "Cube.hpp"
#include "BFS_Solver.hpp"
#include <array>
#include <vector>
#include <string>

class Menu {
public:
  Menu();
  ~Menu();

  Menu(const Menu &) = delete;
  Menu &operator=(const Menu &) = delete;
  void run();

private:
  int readChoice();
  void draw() const;
  void drawCube() const;
  void drawText(const char *text, int x, int y, int size, Color color) const;

  Cube cube;
  Font font{};
  int selected = 0;
  bool cubeVisible = false;

  std::vector<CubeMove> solutionPath;

  bool isSolving = false;
  int currentMoveIndex = 0;
  int totalMovimentos ;
  int estadosExplorados;
  std::string textoSolucao = ""; // Guardará a string legível (ex: "U R F'...")
  bool solucaoPronta = false;
  float moveTimer = 0.0f;
};
