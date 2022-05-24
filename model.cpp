#include "model.h"
#include "map.h"
#include <iostream>
#include <string>
Map &Model::getMap() { return map; }
const std::vector<Button> &Model::getButtons() const { return buttons; }
Model::Model(const std::string &mapFileName) : map(mapFileName) {
  buttons.emplace_back("Single game", 250, 350);
  buttons.emplace_back("Exit", 250, 380);
}
void Model::update(float time) {
    for (int col = 0; col < map.getCols(); col++) {
        for (int row = 0; row < map.getRows(); row++) {
            if (map.field[row][col] == BlockType::SMALL_FLOWER) {
                map.time_of_life[row][col] += 0.1 * time;
                map.score_of_coins[row][col] += ((map.time_of_life[row][col] - map.last_time[row][col]) / 5);
                if (map.time_of_life[row][col] > 50) {
                    map.score_of_coins[row][col] -= ((map.time_of_life[row][col] - 50) / 5);
                    map.time_of_life[row][col] -= 50;
                    map.last_time[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('5') - 48);
                }
                else {
                    map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
                }
            }
            else if (map.field[row][col] == BlockType::AVERAGE_FLOWER) {
                map.time_of_life[row][col] += 0.1 * time;
                map.score_of_coins[row][col] += (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 2);
                if (map.time_of_life[row][col] > 50) {
                    map.score_of_coins[row][col] -= (((map.time_of_life[row][col] - 50) / 5) * 2);
                    map.time_of_life[row][col] -= 50;
                    map.last_time[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('6') - 48);
                }
                else {
                    map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
                }
            }
            else if (map.field[row][col] == BlockType::BIG_FLOWER) {
                map.time_of_life[row][col] += 0.1 * time;
                map.score_of_coins[row][col] += (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 4);
                map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
            }
        }
    }
}