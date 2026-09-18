#include "Cube.hpp"
#include <algorithm>
#include <sstream>



Color Cube::getStickerColor(int x, int y, int z, char axis) const {
    for (const auto& p : pieces) {
        if (p.x == x && p.y == y && p.z == z) {
            if (axis == 'X') return p.colorX;
            if (axis == 'Y') return p.colorY;
            if (axis == 'Z') return p.colorZ;
        }
    }
    return Color::White; 
}

Cube::Cube() {
    // Solved state configuration
    // X=0: Orange, X=1: Red
    // Y=0: Yellow, Y=1: White
    // Z=0: Blue, Z=1: Green

    pieces[0] = {0, 0, 0, Color::Orange, Color::Yellow, Color::Blue};
    pieces[1] = {1, 0, 0, Color::Red,    Color::Yellow, Color::Blue};
    pieces[2] = {0, 1, 0, Color::Orange, Color::White,  Color::Blue};
    pieces[3] = {1, 1, 0, Color::Red,    Color::White,  Color::Blue};
    pieces[4] = {0, 0, 1, Color::Orange, Color::Yellow, Color::Green};
    pieces[5] = {1, 0, 1, Color::Red,    Color::Yellow, Color::Green};
    pieces[6] = {0, 1, 1, Color::Orange, Color::White,  Color::Green};
    pieces[7] = {1, 1, 1, Color::Red,    Color::White,  Color::Green};
}

char Cube::colorToChar(Color c) {
    switch (c) {
        case Color::White:  return 'W';
        case Color::Yellow: return 'Y';
        case Color::Green:   return 'G';
        case Color::Blue:    return 'B';
        case Color::Red:     return 'R';
        case Color::Orange:  return 'O';
        default:             return '?';
    }
}

void Cube::moveRightClockwise() {
    for (auto& p : pieces) {
        if (p.x == 1) {
            int oldY = p.y;
            int oldZ = p.z;
            p.y = oldZ;
            p.z = 1 - oldY;
            std::swap(p.colorY, p.colorZ);
        }
    }
}

void Cube::moveRightCounterclockwise() {
    for (auto& p : pieces) {
        if (p.x == 1) {
            int oldY = p.y;
            int oldZ = p.z;
            p.y = 1 - oldZ;
            p.z = oldY;
            std::swap(p.colorY, p.colorZ);
        }
    }
}

void Cube::moveUpperClockwise() {
    for (auto& p : pieces) {
        if (p.y == 1) {
            int oldX = p.x;
            int oldZ = p.z;
            p.x = oldZ;
            p.z = 1 - oldX;
            std::swap(p.colorX, p.colorZ);
        }
    }
}

void Cube::moveUpperCounterclockwise() {
    for (auto& p : pieces) {
        if (p.y == 1) {
            int oldX = p.x;
            int oldZ = p.z;
            p.x = 1 - oldZ;
            p.z = oldX;
            std::swap(p.colorX, p.colorZ);
        }
    }
}

void Cube::moveFrontClockwise() {
    for (auto& p : pieces) {
        if (p.z == 1) {
            int oldX = p.x;
            int oldY = p.y;
            p.x = 1 - oldY;
            p.y = oldX;
            std::swap(p.colorX, p.colorY);
        }
    }
}

void Cube::moveFrontCounterclockwise() {
    for (auto& p : pieces) {
        if (p.z == 1) {
            int oldX = p.x;
            int oldY = p.y;
            p.x = oldY;
            p.y = 1 - oldX;
            std::swap(p.colorX, p.colorY);
        }
    }
}

bool Cube::isSolved() const {
    Color colorX0, colorX1, colorY0, colorY1, colorZ0, colorZ1;
    bool firstX0 = true, firstX1 = true, firstY0 = true, firstY1 = true, firstZ0 = true, firstZ1 = true;

    for (const auto& p : pieces) {
        if (p.x == 0) {
            if (firstX0) { colorX0 = p.colorX; firstX0 = false; }
            else if (p.colorX != colorX0) return false;
        }
        if (p.x == 1) {
            if (firstX1) { colorX1 = p.colorX; firstX1 = false; }
            else if (p.colorX != colorX1) return false;
        }
        if (p.y == 0) {
            if (firstY0) { colorY0 = p.colorY; firstY0 = false; }
            else if (p.colorY != colorY0) return false;
        }
        if (p.y == 1) {
            if (firstY1) { colorY1 = p.colorY; firstY1 = false; }
            else if (p.colorY != colorY1) return false;
        }
        if (p.z == 0) {
            if (firstZ0) { colorZ0 = p.colorZ; firstZ0 = false; }
            else if (p.colorZ != colorZ0) return false;
        }
        if (p.z == 1) {
            if (firstZ1) { colorZ1 = p.colorZ; firstZ1 = false; }
            else if (p.colorZ != colorZ1) return false;
        }
    }
    return true;
}

std::string Cube::getStateString() const {
    std::stringstream ss;
    for (const auto& p : pieces) {
        ss << p.x << p.y << p.z;
        ss << colorToChar(p.colorX) << colorToChar(p.colorY) << colorToChar(p.colorZ);
    }
    return ss.str();
}
 