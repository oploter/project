#ifndef VARS_H
#define VARS_H
#include <vector>
#include <unordered_map>
#include <string>
enum class BlockType { CONCRETE = 0, GREEN, WATER };
enum class PlayerDir { DOWN = 0, UP, LEFT, RIGHT, STILL};
inline std::vector<std::string> dirStr = {"pl_down", "pl_up", "pl_left", "pl_right", "pl_down"};
const int CellSize = 10;
const int BlockSize = 50;
const float PlayerH = static_cast<float>(95)/BlockSize;
#endif