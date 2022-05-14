#ifndef PROJECT_MAP_H
#define PROJECT_MAP_H

#include <vector>
#include <unordered_map>
#include <string>


struct Map{
public:
    static const int BlockSize;
    static const int CellSize;
    static const float HalfBlockSize;

    Map() : player(), plant_r(50.0 / CellSize){}
    Map(const std::string& file_name);
    void updateBullets(sf::RenderTarget* window = nullptr);
    void updatePlants(sf::RenderTarget* window = nullptr);
    float map_to_pl(int cord);
    Player player;

protected:
    friend struct ClientGame;
    friend struct ServerGame;
    std::vector<std::vector<Block>> field;
    std::vector<std::vector<float>> bullets;
    std::vector<std::pair<float, float>> plants;
    std::vector<int> plants_cnts;
    int block_rows, block_cols;
    int cell_rows, cell_cols;
    float plant_r;
};
// end Map

#endif //PROJECT_MAP_H
