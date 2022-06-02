#ifndef MAP_H
#define MAP_H
#include "vars.h"
#include <string>
#include <vector>
struct Map {
private:
  int rows = 0;
  int cols = 0;

public:
  std::vector<std::vector<BlockType>> field;
  std::vector<std::vector<int>> time_of_life;
  std::vector<std::vector<int>> score_of_coins;
  std::vector<std::vector<int>> last_time;
  std::vector<std::vector<int>> life_of_flowers;

  Map(int rows_, int cols_);
  Map(const std::string &file_name);
  int getRows() const;
  int getCols() const;
  BlockType at(int row, int col) const;
};
#endif
