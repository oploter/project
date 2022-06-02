#include "model.h"
#include "map.h"
#include "vars.h"
#include <iostream>
#include <string>
Map &Model::getMap() { return map; }
Player &Model::getPlayer() { return plr; }
const std::vector<Button> &Model::getButtons() const { return buttons; }
Model::Model(const std::string &mapFileName) : map(mapFileName), plr("hero.png",0,0,25.0, 41.75), state(GameState::MENU) {
    buttons.emplace_back(L"Одиночная игра", 250, 350);
    buttons.emplace_back(L"Выход", 250, 380);
}
void Model::update(float time) {
    switch (map.at((plr.y + plr.h / 2) / BlockSize, (plr.x + plr.w / 2) / BlockSize))
    {
        case BlockType::WATER:
            if(plr.leftOnWater <= 0){
                state = GameState::DIED;
                plr.life = false;
                return;
            }
            break;
        case BlockType::CONCRETE:
            plr.x -= plr.dx * time;
            plr.y -= plr.dy * time;
            break;
        default:
            plr.leftOnWater = 3;
            break;
    }
    for (int col = 0; col < map.getCols(); col++) {
        for (int row = 0; row < map.getRows(); row++) {
            if (map.field[row][col] == BlockType::SMALL_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 20) {
                    map.life_of_flowers[row][col] += 0.1 * time;
                }
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
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 40) {
                    map.life_of_flowers[row][col] += 0.1 * time;
                }
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
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 60) {
                    map.life_of_flowers[row][col] += 0.1 * time;
                }
                map.time_of_life[row][col] += 0.1 * time;
                map.score_of_coins[row][col] += (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 4);
                map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
            }
        }
    }
}

