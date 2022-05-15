#ifndef MAP_H
#define MAP_H
#include <vector>
#include <string>
#include "vars.h"
struct Map{
private:
    int rows = 0;
    int cols = 0;
    std::vector<std::vector<BlockType>> field;
public:
    Map(int rows_, int cols_);
    Map(const std::string& file_name);
    int getRows() const;
    int getCols() const;
    BlockType at(int row, int col) const;
};
#endif