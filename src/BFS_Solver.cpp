
#include "Cube.hpp" 
#include "BFS_Solver.hpp"
#include "Menu.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <queue>
#include <stdexcept>

inline char colorToChar(Color c) { 
    if (c.r == 230 && c.g == 130 && c.b == 40)  return 'O'; // Laranja
    if (c.r == 200 && c.g == 55  && c.b == 55)  return 'R'; // Vermelho
    if (c.r == 235 && c.g == 200 && c.b == 40)  return 'Y'; // Amarelo
    if (c.r == 240 && c.g == 240 && c.b == 240) return 'W'; // Branco
    if (c.r == 45  && c.g == 105 && c.b == 200) return 'B'; // Azul
    if (c.r == 40  && c.g == 170 && c.b == 90)  return 'G'; // Verde
    return '?';
}

// Gera a assinatura de memória, ou seja, crio o estado a partir de todos os indices que descrevem o cubo atual, desde sua posição á sua cor
inline std::string getStateString(const Cube& cube) {
    std::string state = "";
    state.reserve(48); // Aloca espaço exato na RAM para 8 peças * 6 caracteres
    
    for (const auto& cubie : cube.getCubies()) {
        // Converte o int (0 ou 1) diretamente para o caractere char ('0' ou '1')
        state += static_cast<char>(cubie.position[0] + '0');
        state += static_cast<char>(cubie.position[1] + '0');
        state += static_cast<char>(cubie.position[2] + '0');
        
        state += colorToChar(cubie.colors[0]);
        state += colorToChar(cubie.colors[1]);
        state += colorToChar(cubie.colors[2]);
    }
    return state;
}

bool Cube::isSolved() const {
  for (int axis = 0; axis < 3; ++axis) {
    for (int layer = 0; layer < 2; ++layer) {
      Color faceColor{0,0,0,0};
      bool firstCubie = true;

      for (const Cubie &cubie : cubies) {
        if (cubie.position[axis] != layer) {
          continue;
        }

        if (firstCubie) {
          faceColor = cubie.colors[axis];
          firstCubie = false;
        } else {
            if (faceColor.r != cubie.colors[axis].r || 
                faceColor.g != cubie.colors[axis].g || 
                faceColor.b != cubie.colors[axis].b) {
                return false;
        }
      }
    }
  }

}
return true;
}



std::vector<CubeMove> solveCubeBFS(Cube initialCube, int &qtd_movimentos, int &nosvisitados) {
    std::queue<BFSNode> queue;
    std::unordered_set<std::string> visited;
    int cont = 0;  //Contador de estados visitados

    const size_t MAX_MOV = 12;
    const size_t MAX_NOS = 2000000;

    //  Prepara a busca iniciando com o estado atual do cubo
    queue.push({initialCube, {}});
    visited.insert(getStateString(initialCube));

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

        // Projetamos os 12 movimentos possíveis e registramos seus estados
        for (int axis = 0; axis < 3; ++axis) {
                int layer = 1;
                for (bool clockwise : {true, false}) {
                    
                    // Copia o cubo atual para testar o movimento
                    Cube nextCube = current.state;
                    nextCube.rotate(axis, layer, clockwise);

                    std::string nextStateString = getStateString(nextCube);

            
                    if (visited.find(nextStateString) == visited.end()) {
                        visited.insert(nextStateString);
                        
                        BFSNode nextNode;
                        nextNode.state = nextCube;
                        nextNode.path = current.path; // Copia o caminho feito até aqui
                        nextNode.path.push_back({axis,layer,clockwise}); // Adiciona o novo passo

                        cont++; // Contagem de nós visitados

                        queue.push(nextNode);
                    }
                }
        }
    }
    qtd_movimentos = 0;
    nosvisitados = cont;
    return {}; 
}