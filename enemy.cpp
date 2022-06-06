#include "enemy.h"
#include "map.h"
#include "model.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <cassert>
#include <iostream>
#include <queue>
#include <random>
#include <utility>
#include <vector>
bool checkIsPlant(BlockType t);
std::mt19937 rng_enemy(1729); 
Enemy::Enemy(Map &map, int id_)
    : pos({-3 * BlockSize, BlockSize + ((double)rng_enemy() / std::mt19937::max())*(map.getCols() * BlockSize - 2 * BlockSize)}), id(id_) {
    updateMapPos();
}
void Enemy::updateMapPos(){
    mapPos = {(pos.first + h / 2) / BlockSize,
              (pos.second + w / 2) / BlockSize};
}
void Enemy::setPosition(std::pair<float, float> pos_,
                        std::pair<float, float> dest_, Map &map) {
    pos = pos_;
    dest = dest_;
    mem.resize(map.getCols(),
               std::vector<std::pair<int, bool>>(map.getRows(), {-1, false}));
    findPath(pos_.first / BlockSize, pos_.second / BlockSize,
             dest_.first / BlockSize, dest_.second / BlockSize, path, map);
}
float distance(std::pair<float, float> lhs, std::pair<float, float> rhs);
void printType(BlockType t);
void Enemy::findPath(int fromRow, int fromCol, int toRow, int toCol,
                     std::vector<std::pair<int, int>> &p, Map &map) {
    std::cout << "find path " << fromRow << ' ' << fromCol << ' ' << toRow
              << ' ' << toCol << ' ' << p.size();
    std::queue<std::pair<int, int>> q;
    for (int r = 0; r < map.getRows(); r++) {
        for (int c = 0; c < map.getCols(); c++) {
            mem[r][c] = {-1, false};
        }
    }
    q.push({fromRow, fromCol});
    bool found = false;
    while (!found && !q.empty()) {
        std::pair<int, int> currPos = q.front();
        q.pop();
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (std::abs(dx) == std::abs(dy)) {
                    continue;
                }
                std::pair<int, int> newPos = {currPos.first + dy,
                                              currPos.second + dx};
                if (!(0 <= newPos.first && newPos.first < map.getRows()) ||
                    !(0 <= newPos.second && newPos.second < map.getCols())) {
                    continue;
                }
                if (newPos.first == toRow && newPos.second == toCol) {
                    mem[toRow][toCol] = {
                        (dx == 0 ? (dy == -1 ? 0 : 1) : (dx == -1 ? 2 : 3)),
                        true};
                    found = true;
                    break;
                }
                BlockType t = map.at(newPos.first, newPos.second);
                if (t == BlockType::CONCRETE || t == BlockType::WATER ||
                    mem[newPos.first][newPos.second].second == true) {
                    continue;
                }
                mem[newPos.first][newPos.second] = {
                    (dx == 0 ? (dy == -1 ? 0 : 1) : (dx == -1 ? 2 : 3)), true};
                q.push(newPos);
            }
        }
    }
    if (!found) {
        std::cout << "didn't find " << fromRow << ' ' << fromCol << ' ' << toRow
                  << ' ' << toCol << " " << p.size() << "\n";
        return;
    }
    std::pair<int, int> currPos = {toRow, toCol};
    p.push_back(currPos);
    while ((currPos.first != fromRow) || (currPos.second != fromCol)) {
        int d = mem[currPos.first][currPos.second].first;
        if (d < 2) {
            currPos.first += (d == 0 ? 1 : -1);
        } else {
            currPos.second += (d == 2 ? 1 : -1);
        }
        p.push_back(currPos);
    }
}
Direction Enemy::getDirection() const { return dir; }
Action Enemy::getAction() const { return action; }
std::pair<float, float> Enemy::getPos() const { return pos; }
std::pair<float, float> Enemy::getDest() const { return dest; }
std::pair<int, int> Enemy::getMapPos() const { return mapPos; }
Zombie::Zombie(Map &map, int id_) : Enemy(map, id_) {}
Zombie::Zombie(std::pair<float, float> pos_, std::pair<float, float> dest_, Map& map){
    setPosition(pos_, dest_, map);
}
void Zombie::update(float time, Model &model) {
    if(hp <= 0){
        alive = false;
        return;
    }
    time /= 10'000;
    Map &map = model.getMap();
    if (action == Action::ENTRY) {
        //assert(0 <= mapPos.second && mapPos.second < map.getCols());
        //assert(pos.first < 0 || (0 <= mapPos.first && mapPos.first < map.getRows()));
        if (pos.first < BlockSize ||
            map.at(mapPos.first, mapPos.second) == BlockType::CONCRETE ||
            map.at(mapPos.first, mapPos.second) == BlockType::WATER) {
            pos.first += time * speed;
            dir = Direction::DOWN;
        } else {
            setPosition(pos, model.randPoint(true), map);
            action = Action::RANDOM;
        }
        updateMapPos();
        return;
    }
    updateActionAndDir(map);
    if (action == Action::ATTACK) {
        if (!checkIsPlant(map.at(mapPos.first, mapPos.second))) {
            action = Action::RANDOM;
            setPosition(pos, model.randPoint(true), map);
            return;
        }
        if (timeNextAttack > 0) {
            timeNextAttack -= time;
            return;
        }
        map.life_of_flowers[mapPos.first][mapPos.second] -= force;
        map.time_hurt[mapPos.first][mapPos.second] = 0.5;
        timeNextAttack = 1.5;
    } else if (action == Action::RANDOM) {
        switch (dir) {
        case Direction::UP:
            pos.first -= speed * time;
            break;
        case Direction::DOWN:
            pos.first += speed * time;
            break;
        case Direction::LEFT:
            pos.second -= speed * time;
            break;
        case Direction::RIGHT:
            pos.second += speed * time;
            break;
        default:
            break;
        }
        updateMapPos();
        if (path.size() == 0) {
            dir = Direction::STOP;
            setPosition(pos, model.randPoint(true), map);
        } else {
            std::pair<float, float> nextPos = {
                BlockSize / 2 + path.back().first * BlockSize,
                BlockSize / 2 + path.back().second * BlockSize};
            bool ok_x = (std::max(std::abs(nextPos.second - pos.second),
                                  std::abs(nextPos.second - pos.second - w)) <=
                         BlockSize / 2);
            bool ok_y = (std::max(std::abs(nextPos.first - pos.first),
                                  std::abs(nextPos.first - pos.first - h)) <=
                         BlockSize / 2);
            //assert(ok_x || ok_y);
            if (ok_x && ok_y) {
                dir = Direction::STOP;
                path.pop_back();
            } else if (!ok_x) {
                dir = ((std::abs(nextPos.second - pos.second) >=
                        std::abs(nextPos.second - pos.second - w))
                           ? Direction::RIGHT
                           : Direction::LEFT);
            } else {
                dir = ((std::abs(nextPos.first - pos.first) >=
                        std::abs(nextPos.first - pos.first - h))
                           ? Direction::DOWN
                           : Direction::UP);
            }
        }
    }
    if(hp <= 0){
        alive = false;
    }
}
void Zombie::updateActionAndDir(Map &map) {
    BlockType t = map.at(mapPos.first, mapPos.second);
    if (checkIsPlant(t)) {
        action = Action::ATTACK;
        dir = Direction::STOP;
    }
}
bool Enemy::isAlive() const{
    return alive;
}