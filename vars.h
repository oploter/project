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
enum class Direction { UP = 0, DOWN, LEFT, RIGHT, STOP };
enum class Action { RANDOM, PURSUIT, ATTACK, ENTRY };
const int CellSize = 10;
const int BlockSize = 50;
const float PlayerSpeed = BlockSize * 2;
#endif