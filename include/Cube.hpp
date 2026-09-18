#pragma once

#include <array>
#include <string>
#include <vector>

enum class Color {
    White,
    Yellow,
    Green,
    Blue,
    Red,
    Orange
};

struct Cubie {
    int x, y, z;

    Color colorX;
    Color colorY;
    Color colorZ;
};

class Cube {
public:
    std::array<Cubie, 8> pieces;

    Cube();

    // Movements
    void moveRightClockwise();
    void moveRightCounterclockwise();
    void moveUpperClockwise();
    void moveUpperCounterclockwise();
    void moveFrontClockwise();
    void moveFrontCounterclockwise();

    // Utility
    bool isSolved() const;
    Color getStickerColor(int x, int y, int z, char axis) const;
    std::string getStateString() const;
    static char colorToChar(Color c);
};
