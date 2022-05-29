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
const int CellSize = 10;
const int BlockSize = 50;
#endif