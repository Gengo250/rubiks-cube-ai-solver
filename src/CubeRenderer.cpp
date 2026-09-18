#include "CubeRenderer.hpp"
#include "Cube.hpp"
#include <array>
#include <cstddef>
#include <iostream>


char CubeRenderer::colorToChar(Color color) {
    switch (color) {
        case Color::White:  return 'W';
        case Color::Yellow: return 'Y';
        case Color::Green:  return 'G';
        case Color::Blue:   return 'B';
        case Color::Red:    return 'R';
        case Color::Orange: return 'O';
        default:            return '?';
    }
}

void CubeRenderer::render(const Cube& cube) {
    // O lambda agora converte (linha, coluna) 2D do console para (x, y, z) 3D do cubo
    auto getC = [&](char side, int row, int col) {
        switch (side) {
            // Topo (Y = 1). A linha 0 é o fundo (Z=0), a linha 1 é a frente (Z=1).
            case 'U': return cube.getStickerColor(col, 1, row, 'Y');
            
            // Base (Y = 0). A linha 0 toca a frente (Z=1), a linha 1 toca o fundo (Z=0).
            case 'D': return cube.getStickerColor(col, 0, 1 - row, 'Y');
            
            // Frente (Z = 1).
            case 'F': return cube.getStickerColor(col, 1 - row, 1, 'Z');
            
            // Fundo (Z = 0). Olhando por trás, a esquerda da tela é o X=1 do cubo.
            case 'B': return cube.getStickerColor(1 - col, 1 - row, 0, 'Z');
            
            // Esquerda (X = 0). A esquerda da tela é o fundo (Z=0).
            case 'L': return cube.getStickerColor(0, 1 - row, col, 'X');
            
            // Direita (X = 1). A esquerda da tela é a frente (Z=1).
            case 'R': return cube.getStickerColor(1, 1 - row, 1 - col, 'X');
        }
        return Color::White;
    };

    // 1. Renderiza o Topo (Upper)
    for (int row = 0; row < 2; ++row) {
        std::cout << "        ";
        for (int col = 0; col < 2; ++col) {
            std::cout << colorToChar(getC('U', row, col)) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // 2. Renderiza a Faixa Central (Left, Front, Right, Rear)
    char middleFaces[] = {'L', 'F', 'R', 'B'};
    
    for (int row = 0; row < 2; ++row) {
        for (char face : middleFaces) {
            for (int col = 0; col < 2; ++col) {
                std::cout << colorToChar(getC(face, row, col)) << ' ';
            }
            std::cout << "    ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';

    // 3. Renderiza a Base (Lower)
    for (int row = 0; row < 2; ++row) {
        std::cout << "        ";
        for (int col = 0; col < 2; ++col) {
            std::cout << colorToChar(getC('D', row, col)) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}