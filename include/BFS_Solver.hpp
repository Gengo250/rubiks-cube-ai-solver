#pragma once
#include "Cube.hpp"
#include <string>
#include <vector>

struct CubeMove {
    int axis;
    int layer;
    bool clockwise;
};

struct BFSNode {
    Cube state;
    std::vector<CubeMove> path;
    int ultEixo = -1;
    
};

// Retorna agora um vetor de estruturas CubeMove para compatibilidade com o Menu
std::vector<CubeMove> solveCubeBFS(Cube initialCube, int &qtd_movimentos, int &nosvisitados);
inline std::string getStateString(const Cube& cube);
inline char colorToChar(Color c);