#include "model.h"
#include "map.h"
#include "vars.h"
#include <iostream>
#include <string>
bool checkIsPlant(BlockType t) {
    return (t == BlockType::AVERAGE_FLOWER || t == BlockType::BIG_FLOWER ||
            t == BlockType::SMALL_FLOWER);
}
Map &Model::getMap() { return map; }
Player &Model::getPlayer() { return plr; }
const std::vector<Button> &Model::getButtons() const { return buttons; }
Model::Model(const std::string &mapFileName, int numOfEnemies)
    : map(mapFileName), plr("hero.png", 0, 0, 25.0, 41.75),
      state(GameState::MENU) {
    buttons.emplace_back(L"Одиночная игра", 250, 350);
    buttons.emplace_back(L"Выход", 250, 380);
    for (int i = 0; i < numOfEnemies; i++) {
        enemies.emplace_back(std::make_unique<Zombie>(map));
    }
}
void Model::update(float time) {
    for (int col = 0; col < map.getCols(); col++) {
        for (int row = 0; row < map.getRows(); row++) {
            if (checkIsPlant(map.at(row, col))) {
                map.time_hurt[row][col] = std::max(
                    static_cast<float>(0), map.time_hurt[row][col] - time);
            }
            if (map.field[row][col] == BlockType::SMALL_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] =
                        static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 20) {
                    map.life_of_flowers[row][col] += 4 * time;
                }
                map.time_of_life[row][col] += 4 * time;
                map.score_of_coins[row][col] +=
                    ((map.time_of_life[row][col] - map.last_time[row][col]) /
                     5);
                if (map.time_of_life[row][col] > 50) {
                    map.score_of_coins[row][col] -=
                        ((map.time_of_life[row][col] - 50) / 5);
                    map.time_of_life[row][col] -= 50;
                    map.last_time[row][col] = 0;
                    map.field[row][col] =
                        static_cast<BlockType>(static_cast<int>('5') - 48);
                } else {
                    map.last_time[row][col] = map.time_of_life[row][col] -
                                              (map.time_of_life[row][col] % 5);
                }
            } else if (map.field[row][col] == BlockType::AVERAGE_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] =
                        static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 40) {
                    map.life_of_flowers[row][col] += 4 * time;
                }
                map.time_of_life[row][col] += 4 * time;
                map.score_of_coins[row][col] +=
                    (((map.time_of_life[row][col] - map.last_time[row][col]) /
                      5) *
                     2);
                if (map.time_of_life[row][col] > 50) {
                    map.score_of_coins[row][col] -=
                        (((map.time_of_life[row][col] - 50) / 5) * 2);
                    map.time_of_life[row][col] -= 50;
                    map.last_time[row][col] = 0;
                    map.field[row][col] =
                        static_cast<BlockType>(static_cast<int>('6') - 48);
                } else {
                    map.last_time[row][col] = map.time_of_life[row][col] -
                                              (map.time_of_life[row][col] % 5);
                }
            } else if (map.field[row][col] == BlockType::BIG_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] =
                        static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 60) {
                    map.life_of_flowers[row][col] += 4 * time;
                }
                map.time_of_life[row][col] += 4 * time;
                map.score_of_coins[row][col] +=
                    (((map.time_of_life[row][col] - map.last_time[row][col]) /
                      5) *
                     4);
                map.last_time[row][col] = map.time_of_life[row][col] -
                                          (map.time_of_life[row][col] % 5);
            }
        }
    }
    for (std::unique_ptr<Enemy> &enemy_ptr : enemies) {
        enemy_ptr->update(time, *this);
    }
    if(!plr.life){
        state = GameState::DIED;
    }
}
std::pair<float, float> Model::randPoint(bool isGreen) {
    float rand_x, rand_y;
    while (1) {
        rand_x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) *
                 (map.getCols() * BlockSize);
        rand_y = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) *
                 (map.getRows() * BlockSize);
        BlockType t = map.at(rand_y / BlockSize, rand_x / BlockSize);
        if (!isGreen || (t != BlockType::CONCRETE && t != BlockType::WATER)) {
            break;
        }
    }
    return {rand_y, rand_x};
}
std::vector<std::unique_ptr<Enemy>> &Model::getEnemies() { return enemies; }
float distance(std::pair<float, float> lhs, std::pair<float, float> rhs) {
    return (lhs.first - rhs.first) * (lhs.first - rhs.first) +
           (lhs.second - rhs.second) * (lhs.second - rhs.second);
}
void printType(BlockType t) {
    std::cout
        << (t == BlockType::AVERAGE_FLOWER
                ? "AVRG FLWR"
                : (t == BlockType::BIG_FLOWER
                       ? "BIG FLWR"
                       : (t == BlockType::CONCRETE
                              ? "CONCRETE"
                              : (t == BlockType::GREEN
                                     ? "GREEN"
                                     : (t == BlockType::MOLE
                                            ? "MOLE"
                                            : (t == BlockType::SMALL_FLOWER
                                                   ? "SML FLWR"
                                                   : "WATER"))))))
        << "\n";
}