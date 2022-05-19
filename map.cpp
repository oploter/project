#include "map.h"
#include "vars.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
Map::Map(int rows_, int cols_)
    : rows(rows_), cols(cols_),
      field(rows, std::vector<BlockType>(cols, BlockType::GREEN)) {}
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
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            char type;
            file >> type;
            field[row][col] =
                static_cast<BlockType>(static_cast<int>(type) - 48);
        }
    }
}
int Map::getRows() const { return rows; }
int Map::getCols() const { return cols; }
BlockType Map::at(int row, int col) const { return field[row][col]; }
