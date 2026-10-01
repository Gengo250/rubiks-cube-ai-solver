#pragma once

#include "Cube.hpp"

#include <vector>

// Modelo de docs/heuristica-a-estrela.md: a peca 0 fica parada e so as
// camadas layer=1 dos tres eixos giram. Metrica HTM, entao quarto e meia
// volta custam 1 e ha nove movimentos.
namespace heuristic {

inline constexpr int MOVE_COUNT = 9;

// Estado canonico dos sete slots moveis.
struct State {
  int perm = 0; // 0..5039, codigo de Lehmer da permutacao
  int ori = 0;  // 0..2186, rotulo de orientacao por slot, base 3

  int index() const;
  bool solved() const;
};

// Giro do cubo inteiro: as duas camadas do eixo no mesmo sentido absoluto.
struct Spin {
  int axis = 0;
  bool clockwise = true;
};

State solvedState();
State apply(State state, int move);

// move = axis * 3 + tipo, com tipo 0 horario, 1 anti-horario, 2 meia-volta.
const char *moveName(int move);
int moveAxis(int move);
bool moveIsClockwise(int move);
bool moveIsHalf(int move);

// Giros do cubo inteiro que levam a peca 0 de volta ao slot 0 com a
// orientacao original. Vazio quando ela ja esta na posição.
std::vector<Spin> homingSpins(const Cube &cube);

// Exige a peca 0 em casa; aplique homingSpins antes.
State stateOf(const Cube &cube);

// Quatro pattern databases: grupos {1,2,3} e {4,5,6,7}, permutacao e
// orientacao. 79 KiB no total, construidas uma vez.
void build();
int value(State state);

} // namespace heuristic
