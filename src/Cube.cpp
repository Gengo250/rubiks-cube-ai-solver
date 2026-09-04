#include "Cube.hpp"
#include <cstddef>

Cube::Cube() {
  auto upper = static_cast<std::size_t>(Face::Upper);
  faces[upper].fill(Color::White);

  auto lower = static_cast<std::size_t>(Face::Lower);
  faces[lower].fill(Color::Yellow);

  auto front = static_cast<std::size_t>(Face::Front);
  faces[front].fill(Color::Green);

  auto rear = static_cast<std::size_t>(Face::Rear);
  faces[rear].fill(Color::Blue);

  auto right = static_cast<std::size_t>(Face::Right);
  faces[right].fill(Color::Red);

  auto left = static_cast<std::size_t>(Face::Left);
  faces[left].fill(Color::Orange);
}

Color Cube::getColor(Face face, std::size_t position) const {
  std::size_t faceIndex = static_cast<std::size_t>(face);

  return faces[faceIndex][position];
}

void Cube::moveFrontClockwise() {
  std::size_t front = static_cast<std::size_t>(Face::Front);
  std::size_t upper = static_cast<std::size_t>(Face::Upper);
  std::size_t lower = static_cast<std::size_t>(Face::Lower);
  std::size_t right = static_cast<std::size_t>(Face::Right);
  std::size_t left = static_cast<std::size_t>(Face::Left);

  auto oldFront = faces[front];

  faces[front][0] = oldFront[2];
  faces[front][1] = oldFront[0];
  faces[front][2] = oldFront[3];
  faces[front][3] = oldFront[1];

  Color upperBottomLeft = faces[upper][2];
  Color upperBottomRight = faces[upper][3];

  faces[upper][2] = faces[left][3];
  faces[upper][3] = faces[left][1];

  faces[left][1] = faces[lower][0];
  faces[left][3] = faces[lower][1];

  faces[lower][0] = faces[right][2];
  faces[lower][1] = faces[right][0];

  faces[right][0] = upperBottomLeft;
  faces[right][2] = upperBottomRight;
}

void Cube::moveLeftClockwise() {
  std::size_t left = static_cast<std::size_t>(Face::Left);
  std::size_t upper = static_cast<std::size_t>(Face::Upper);
  std::size_t lower = static_cast<std::size_t>(Face::Lower);
  std::size_t front = static_cast<std::size_t>(Face::Front);
  std::size_t rear = static_cast<std::size_t>(Face::Rear);

  auto oldLeft = faces[left];

  faces[left][0] = oldLeft[2];
  faces[left][1] = oldLeft[0];
  faces[left][2] = oldLeft[3];
  faces[left][3] = oldLeft[1];

  Color upperRearLeft = faces[upper][0];
  Color upperFrontLeft = faces[upper][2];

  faces[upper][0] = faces[rear][3];
  faces[upper][2] = faces[rear][1];

  faces[rear][3] = faces[lower][0];
  faces[rear][1] = faces[lower][2];

  faces[lower][0] = faces[front][0];
  faces[lower][2] = faces[front][2];

  faces[front][0] = upperRearLeft;
  faces[front][2] = upperFrontLeft;
}


void Cube::moveFrontCounterclockwise() {
  std::size_t front = static_cast<std::size_t>(Face::Front);
  std::size_t upper = static_cast<std::size_t>(Face::Upper);
  std::size_t lower = static_cast<std::size_t>(Face::Lower);
  std::size_t right = static_cast<std::size_t>(Face::Right);
  std::size_t left = static_cast<std::size_t>(Face::Left);


  auto oldFront = faces[front];
  auto oldUpper = faces[upper];
  auto oldLower = faces[lower];
  auto oldRight = faces[right];
  auto oldLeft  = faces[left];

  faces[front][0] = oldFront[1];
  faces[front][1] = oldFront[3];
  faces[front][3] = oldFront[2];
  faces[front][2] = oldFront[0];
  

  faces[upper][2] = oldRight[0];
  faces[upper][3] = oldRight[2];

  faces[left][1] = oldUpper[3];
  faces[left][3] = oldUpper[2];

  faces[lower][0] = oldLeft[1];
  faces[lower][1] = oldLeft[3];

  faces[right][0] = oldLower[0];
  faces[right][2] = oldLower[1];
}

void Cube::moveRightClockwise(){
  std::size_t front = static_cast<std::size_t>(Face::Front);
  std::size_t upper = static_cast<std::size_t>(Face::Upper);
  std::size_t lower = static_cast<std::size_t>(Face::Lower);
  std::size_t right = static_cast<std::size_t>(Face::Right);
  std::size_t left = static_cast<std::size_t>(Face::Left);
  std::size_t rear = static_cast<std::size_t>(Face::Rear);

  Color temp = faces[right][0];
  faces[right][0] = faces[right][2];
  faces[right][2] = faces[right][3];
  faces[right][3] = faces[right][1];
  faces[right][1] = temp;
  

  Color templowerH = faces[lower][1];
  Color templowerL = faces[lower][3];


  faces[lower][1] = faces[rear][1];
  faces[lower][3] = faces[rear][3];

  faces[rear][1] = faces[upper][1];
  faces[rear][3] = faces[upper][3];

  faces[upper][1] = faces[front][1];
  faces[upper][3] = faces[front][3];
  
  faces[front][1] = templowerH;
  faces[front][3] = templowerL;

}

void Cube::moveRightCounterclockwise(){

  std::size_t front = static_cast<std::size_t>(Face::Front);
  std::size_t upper = static_cast<std::size_t>(Face::Upper);
  std::size_t lower = static_cast<std::size_t>(Face::Lower);
  std::size_t right = static_cast<std::size_t>(Face::Right);
  std::size_t left = static_cast<std::size_t>(Face::Left);
  std::size_t rear = static_cast<std::size_t>(Face::Rear);


  Color temp = faces[right][0];
  faces[right][0] = faces[right][1];
  faces[right][1] = faces[right][3];
  faces[right][3] = faces[right][2];
  faces[right][2] = temp;

  Color tempupperH = faces[upper][1];
  Color tempupperL = faces[upper][3];

  faces[upper][1] = faces[rear][1];
  faces[upper][3] = faces[rear][3];
  
  faces[rear][1] = faces[lower][1];
  faces[rear][3] = faces[lower][3];

  faces[lower][1] = faces[front][1];
  faces[lower][3] = faces[front][3];

  faces[front][1] = tempupperH;
  faces[front][3] = tempupperL;

}



