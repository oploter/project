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

  Map(int rows_, int cols_);
  Map(const std::string &file_name);
  int getRows() const;
  int getCols() const;
  BlockType at(int row, int col) const;
};
#endif
