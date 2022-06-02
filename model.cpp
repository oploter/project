#include "model.h"
#include "map.h"
#include "vars.h"
#include <iostream>
#include <string>
#include <random>
#include <algorithm>
Map &Model::getMap() { return map; }
Player &Model::getPlayer() { return plr; }
const std::vector<Button> &Model::getButtons() const { return buttons; }
Model::Model(const std::string &mapFileName, int numOfEnemies)
    : map(mapFileName), plr("hero.png", 0, 0, 25.0, 41.75),
      state(GameState::MENU) {
  buttons.emplace_back(L"Одиночная игра", 250, 350);
  buttons.emplace_back(L"Выход", 250, 380);
  for(int i = 0; i < numOfEnemies; i++){
      enemies.emplace_back(randPoint(true), randPoint(true), map);
  }
}
void Model::update(float tm) {
  switch (map.at((plr.getY() + plr.h / 2) / BlockSize,
                 (plr.getX() + plr.w / 2) / BlockSize)) {
  case BlockType::WATER:
    if (plr.leftOnWater <= 0) {
      state = GameState::DIED;
      plr.life = false;
      return;
    }
    break;
  case BlockType::CONCRETE:
    plr.x -= plr.dx * tm;
    plr.y -= plr.dy * tm;
    break;
  default:
    plr.leftOnWater = 3;
    break;
  }
  for (int col = 0; col < map.getCols(); col++) {
    for (int row = 0; row < map.getRows(); row++) {
      map.time_hurt[row][col] = std::max(static_cast<float>(0), (map.time_hurt[row][col]) - (tm / 100'000));
      if (map.field[row][col] == BlockType::SMALL_FLOWER) {
        if (map.life_of_flowers[row][col] <= 0) {
          map.life_of_flowers[row][col] = 0;
          map.field[row][col] = BlockType::GREEN;
        }
        if (map.life_of_flowers[row][col] < 20) {
          map.life_of_flowers[row][col] += 0.002 * tm;
        }
        map.time_of_life[row][col] += 0.0002 * tm;
        std::cout << map.time_of_life[row][col] << "\n";
        map.score_of_coins[row][col] +=
            ((map.time_of_life[row][col] - map.last_time[row][col]) / 5);
        if (map.time_of_life[row][col] > 50) {
          map.score_of_coins[row][col] -=
              ((map.time_of_life[row][col] - 50) / 5);
          map.time_of_life[row][col] -= 50;
          map.last_time[row][col] = 0;
          map.field[row][col] = BlockType::AVERAGE_FLOWER;
        } else {
          map.last_time[row][col] =
              map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
        }
      } else if (map.field[row][col] == BlockType::AVERAGE_FLOWER) {
        if (map.life_of_flowers[row][col] <= 0) {
          map.life_of_flowers[row][col] = 0;
          map.field[row][col] = BlockType::GREEN;
        }
        if (map.life_of_flowers[row][col] < 40) {
          map.life_of_flowers[row][col] += 0.002 * tm;
        }
        map.time_of_life[row][col] += 0.0002 * tm;
        map.score_of_coins[row][col] +=
            (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 2);
        if (map.time_of_life[row][col] > 50) {
          map.score_of_coins[row][col] -=
              (((map.time_of_life[row][col] - 50) / 5) * 2);
          map.time_of_life[row][col] -= 50;
          map.last_time[row][col] = 0;
          map.field[row][col] = BlockType::BIG_FLOWER;
        } else {
          map.last_time[row][col] =
              map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
        }
      } else if (map.field[row][col] == BlockType::BIG_FLOWER) {
        if (map.life_of_flowers[row][col] <= 0) {
          map.life_of_flowers[row][col] = 0;
          map.field[row][col] = BlockType::GREEN;
        }
        if (map.life_of_flowers[row][col] < 60) {
          map.life_of_flowers[row][col] += 0.002 * tm;
        }
        map.time_of_life[row][col] += 0.0002 * tm;
        map.score_of_coins[row][col] +=
            (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 4);
        map.last_time[row][col] =
            map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
      }
    }
  }
  for(Enemy& enemy : enemies){
    enemy.update(tm, map);
  }
}
std::pair<float, float> Model::randPoint(bool isGreen){
    float rand_x, rand_y;
    while(1){
        rand_x = ( static_cast<float>(rand()) / static_cast<float>(RAND_MAX) ) * (map.getCols() * BlockSize);
        rand_y = ( static_cast<float>(rand()) / static_cast<float>(RAND_MAX) ) * (map.getRows() * BlockSize);
        BlockType t = map.at(rand_y / BlockSize, rand_x / BlockSize);
        if(!isGreen || (t != BlockType::CONCRETE && t != BlockType::WATER)){
            break;
        }
    }
    return {rand_y, rand_x};
}
float distance(std::pair<float, float> lhs, std::pair<float, float> rhs) {
    return (lhs.first - rhs.first) * (lhs.first - rhs.first) + (lhs.second - rhs.second) * (lhs.second - rhs.second);
}
std::vector<Enemy>& Model::getEnemies(){
    return enemies;
}