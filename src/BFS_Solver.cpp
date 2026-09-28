
#include "Cube.hpp" 
#include "BFS_Solver.hpp"
#include "Menu.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <queue>
#include <stdexcept>
#include <cstdint>


// Gera a assinatura de memória, ou seja, crio o estado a partir de todos os indices que descrevem o cubo atual, desde sua posição á sua cor
inline uint64_t colorToBits(Color c) {
    if (c.r == 230) return 0; // Laranja
    if (c.r == 200) return 1; // Vermelho
    if (c.r == 235) return 2; // Amarelo
    if (c.r == 240) return 3; // Branco
    if (c.r == 45)  return 4; // Azul
    if (c.r == 40)  return 5; // Verde
    return 7;
}


// O estado inteiro compactado em 64 bits (sem alocação de memória)
inline uint64_t getCompactState(const Cube& cube) {
    uint64_t state = 0;
    int shift = 0;
    
    for (const auto& cubie : cube.getCubies()) {
        // Ignora a peça âncora (0,0,0)
        if (cubie.position[0] == 0 && cubie.position[1] == 0 && cubie.position[2] == 0) continue;

        uint64_t pos = (cubie.position[0] << 2) | (cubie.position[1] << 1) | cubie.position[2];
        

        uint64_t cX = colorToBits(cubie.colors[0]);
        uint64_t cY = colorToBits(cubie.colors[1]);
        uint64_t cZ = colorToBits(cubie.colors[2]);

        // Empacota os 12 bits desta peça no número principal
        uint64_t pieceData = (pos << 9) | (cX << 6) | (cY << 3) | cZ;
        state |= (pieceData << shift);
        
        shift += 12; 
    }
    return state;
}




std::vector<CubeMove> solveCubeBFS(Cube initialCube, int &qtd_movimentos, int &nosvisitados) {
    std::queue<BFSNode> queue;
    std::unordered_set<uint64_t> visited;
    int cont = 0;  //Contador de estados visitados

    const size_t MAX_MOV = 15;
    const size_t MAX_NOS = 20000000;

    //  Prepara a busca iniciando com o estado atual do cubo
    queue.push({initialCube, {}});
    visited.insert(getCompactState(initialCube));

    // Loop de expansão para coninuar a movimentação 
    while (!queue.empty()) {
        BFSNode current = queue.front();
        queue.pop();
        // Verificação do cubo resolvido
        if (current.state.isSolved()) {
            qtd_movimentos = current.path.size();
            nosvisitados = cont;
            return current.path; // Retorna a lista de movimentos vitoriosa
        }

        if(current.path.size() >= MAX_MOV){
          continue;
        }
        if(cont >= MAX_NOS){
          break;
        }

      for (int axis = 0; axis < 3; ++axis) {

          if (axis == current.ultEixo) {
            continue; 
          }

        int layer = 1;
        for (bool clockwise : {true, false}) {
            Cube nextCube = current.state;
            nextCube.rotate(axis, layer, clockwise);

            std::uint64_t nextStateString = getCompactState(nextCube);

        if (visited.find(nextStateString) == visited.end()) {
            visited.insert(nextStateString);
            
            BFSNode nextNode;
            nextNode.state = nextCube;
            nextNode.path = current.path; 
            nextNode.path.push_back({axis, layer, clockwise}); 
            nextNode.ultEixo = axis; 

            cont++; 
            queue.push(nextNode);
        }
    }
}
    }
    qtd_movimentos = 0;
    nosvisitados = cont;
    return {}; 
}