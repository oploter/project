#ifndef VARS_H
#define VARS_H
#include <vector>
enum class BlockType {
  CONCRETE = 0,
  GREEN,
  WATER,
  MOLE,
  SMALL_FLOWER,
  AVERAGE_FLOWER,
  BIG_FLOWER
};
enum class GameState { MENU = 0, GAME, DIED };
enum class Direction { LEFT, RIGHT, DOWN, UP, STOP };
enum class Action { RANDOM, ATTACK };
const int CellSize = 10;
const float BlockSize = 50;
#endif