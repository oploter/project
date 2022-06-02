#include "enemy.h"
#include "model.h"
#include "view.h"
#include <utility>
#include <vector>
#include <queue>
#include <cassert>
#include <iostream>
#include <SFML/Graphics.hpp>
Enemy::Enemy(std::pair<float, float> pos_, std::pair<float, float> dest_, Map& map) : pos(pos_){
    sprite.setTexture(*View::get_or_create_texture("enemy", "../img/enemy.png", true));
    setPosition(pos_, dest_, map);
    std::cout << "pos " << (pos.first + h / 2) << ' ' << (pos.second + w / 2) << "\n";
    sprite.setPosition(pos.first, pos.second);
}
void Enemy::setPosition(std::pair<float, float> pos_, std::pair<float, float> dest_, Map& map){
    pos = pos_;
    mapPos = {(pos.first + h / 2) / BlockSize, (pos.second + w / 2) / BlockSize};
    mem.resize(map.getCols(), std::vector<std::pair<int, bool>>(map.getRows(), {-1, false}));
    findPath(pos_.first / BlockSize, pos_.second / BlockSize, dest_.first / BlockSize, dest_.second / BlockSize, path, map);
}
float distance(std::pair<float, float> lhs, std::pair<float, float> rhs);
void Enemy::update(float time, Map& map){
    updateActionAndDir(map);
    if(action == Action::ATTACK){
        if(timeNextAttack > 0){
            timeNextAttack -= time / 100'000;
            return;
        }
        map.life_of_flowers[mapPos.first][mapPos.second] -= force;
        map.time_hurt[mapPos.first][mapPos.second] = 2;
        std::cout << "HURT! " << map.life_of_flowers[mapPos.first][mapPos.second] << "\n";
    }else if(action == Action::RANDOM){
        switch (dir)
        {
        case Direction::LEFT:
            pos.second -= speed * time;
            break;
        case Direction::RIGHT:
            pos.second += speed * time;
            break;  
        case Direction::UP:
            pos.first -= speed * time;
            break;
        case Direction::DOWN:
            pos.first += speed * time;
            break;
        default:
            break;
        }
        if(path.size() == 0){
            dir = Direction::STOP;
        }else{
            std::pair<int, int> nextPos = path.back();
            std::pair<float, float> nextPosPixels = {nextPos.first * BlockSize + BlockSize / 2, nextPos.second * BlockSize + BlockSize / 2};
            bool ok_y = (pos.first <= nextPosPixels.first) && (nextPosPixels.first <= pos.first + h);
            bool ok_x = (pos.second <= nextPosPixels.second) && (nextPosPixels.second <= pos.second + w);
            assert(ok_x || ok_y);
            if(ok_x && ok_y){
                dir = Direction::STOP;
                path.pop_back();
            }else if(!ok_x){
                dir = (pos.second > nextPosPixels.second ? Direction::LEFT : Direction::RIGHT);
            }else{
                dir = (pos.first > nextPosPixels.first ? Direction::UP : Direction::DOWN);
            }
        } 
        //std::cout << (dir == Direction::LEFT ? "LEFT" : (dir == Direction::RIGHT ? "RIGHT" : (dir == Direction::UP ? "UP" : (dir == Direction::DOWN ? "DOWN" : "STOP")))) << "\n";
    }
}
void Enemy::updateActionAndDir(Map& map){
    BlockType t = map.at(mapPos.first, mapPos.second);
    if(t == BlockType::SMALL_FLOWER || t == BlockType::AVERAGE_FLOWER || t == BlockType::BIG_FLOWER){
        action = Action::ATTACK;
    }
}
void Enemy::findPath(int fromRow, int fromCol, int toRow, int toCol, std::vector<std::pair<int, int>>& p, Map& map){
    std::queue<std::pair<int, int>> q;
    q.push({fromRow, fromCol});
    bool found = false;
    while(!found && !q.empty()){
        std::pair<int, int> currPos = q.front();
        q.pop();
        for(int dx = -1; dx <= 1; dx++){
            for(int dy = -1; dy <= 1; dy++){
                if(std::abs(dx) == std::abs(dy)){
                    continue;
                }
                std::pair<int, int> newPos = {currPos.first + dy, currPos.second + dx};
                if(!(0 <= newPos.first && newPos.first < map.getRows()) || !(0 <= newPos.second && newPos.second < map.getCols())){
                    continue;
                }   
                if(newPos.first == toRow && newPos.second == toCol){
                    mem[toRow][toCol] = {(dx == 0 ? (dy == -1 ? 0 : 1) : (dx == -1 ? 2 : 3)), true};
                    found = true;
                    break;
                }
                BlockType t = map.at(newPos.first, newPos.second);
                if(t == BlockType::CONCRETE || t == BlockType::WATER || mem[newPos.first][newPos.second].second == true){
                    continue;
                }
                mem[newPos.first][newPos.second] = {(dx == 0 ? (dy == -1 ? 0 : 1) : (dx == -1 ? 2 : 3)), true};
                q.push(newPos);
            }
        }
    }
    if(!found){
        return;
    }
    std::pair<int, int> currPos = {toRow, toCol};
    p.push_back(currPos);
    while((currPos.first != fromRow) || (currPos.second != fromCol)){
        int d = mem[currPos.first][currPos.second].first;
        if(d < 2){
            currPos.first += (d == 0 ? 1 : -1);
        }else{
            currPos.second += (d == 2 ? 1 : -1);
        }
        p.push_back(currPos);
    }
}
Direction Enemy::getDirection() const{
    return dir;
}
std::pair<float, float> Enemy::getPos() const{
    return pos;
}