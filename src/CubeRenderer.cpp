#include "CubeRenderer.hpp"
#include "Cube.hpp"
#include <array>
#include <cstddef>
#include <iostream>


char CubeRenderer::colorToChar(Color color){
  switch (color) {
    case Color::White:
      return 'W';
    case Color::Yellow:
      return 'Y';
    case Color::Green:
      return 'G';
    case Color::Blue:
      return 'B';
    case Color::Red:
      return 'R';
    case Color::Orange:
      return 'O';
    default:
      return '?';
  }
}

  void CubeRenderer::render(const Cube& cube){

    std::array<Face, 4> sideFaces = {
      Face::Left,
      Face::Front,
      Face::Right,
      Face::Rear
    };

    for (std::size_t row = 0; row < 2; ++row) {
    std::cout << "        ";
      for (std::size_t column = 0; column < 2; ++column) {
        std::size_t position = row * 2 + column;
        Color color = cube.getColor(Face::Upper, position);
        char symbol = colorToChar(color);
        std::cout << symbol << ' '; 
     }
     std::cout << '\n';
    }
    std::cout << '\n';

    for(std::size_t row = 0; row < 2; ++row){
     for(Face face :sideFaces){
      for(std::size_t column = 0; column < 2; ++column){
        std::size_t position = row * 2 + column;
        Color color = cube.getColor(face, position);
        char symbol = colorToChar(color);
        std::cout << symbol << ' '; 
       }
        std::cout << "    ";
      }
      std::cout << '\n';
    }

   std::cout << '\n';

   for (std::size_t row = 0; row < 2; ++row) {
    std::cout << "        ";
      for (std::size_t column = 0; column < 2; ++column) {
        std::size_t position = row * 2 + column;
        Color color = cube.getColor(Face::Lower, position);
        char symbol = colorToChar(color);
        std::cout << symbol << ' '; 
     }
     std::cout << '\n';
    }
    std::cout << '\n';
  }
    

