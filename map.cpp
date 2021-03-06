#include "map.h"
#include "vars.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
Map::Map(int rows_, int cols_)
    : rows(rows_), cols(cols_),
      field(rows, std::vector<BlockType>(cols, BlockType::GREEN)),
      time_of_life(rows, std::vector<int>(cols, 0)),
      score_of_coins(rows, std::vector<int>(cols, 0)),
      last_time(rows, std::vector<int>(cols, 0)) {}
Map::Map(const std::string &file_name) {
  std::ifstream file;
  file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
  try {
    file.open(file_name);
  } catch (...) {
    std::cout << "Cannot open: " << file_name << "\n";
    return;
  }
  file >> rows >> cols;
  field.resize(rows, std::vector<BlockType>(cols, BlockType::GREEN));
  time_of_life.resize(rows, std::vector<int>(cols, 0));
  score_of_coins.resize(rows, std::vector<int>(cols, 0));
  last_time.resize(rows, std::vector<int>(cols, 0));
  life_of_flowers.resize(rows, std::vector<int>(cols, 0));
  map_of_players.resize(rows, std::vector<std::string>(cols, ""));
  time_hurt.resize(rows, std::vector<float>(cols, 0));
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      char type;
      file >> type;
      field[row][col] = static_cast<BlockType>(static_cast<int>(type) - 48);
    }
  }
}
int Map::getRows() const { return rows; }
int Map::getCols() const { return cols; }
BlockType Map::at(int row, int col) const { return field.at(row).at(col); }
