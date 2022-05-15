#ifndef VARS_H
#define VARS_H
#include <vector>
#include <string>
#include <unordered_map>
enum class BlockType{GREEN, CONCRETE, WATER};
const int CellSize = 10;
const std::vector<std::string> texturesToLoad = {"green_block", "concrete_block", "water_block"};
const std::vector<std::string> fontsToLoad = {"basicFont"};
const std::unordered_map<BlockType, std::string> blockTotexture = {{BlockType::GREEN, "green_block"}, {BlockType::CONCRETE, "concrete_block"}, {BlockType::WATER, "water_block"}};
#endif