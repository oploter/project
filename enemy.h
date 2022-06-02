#ifndef ENEMY_H
#define ENEMY_H
#include "map.h"
#include "vars.h"
#include <utility>
#include <vector>
#include <SFML/Graphics.hpp>
struct Enemy{
private:
    float speed = 0.0008;
    float force = 0.1;
    float timeNextAttack = 2;
    std::pair<float, float> pos;
    std::pair<int, int> mapPos;
    std::vector<std::pair<int, int>> path;
    std::vector<std::vector<std::pair<int, bool>>> mem;
    Direction dir = Direction::STOP;
    Action action = Action::RANDOM;
public:
    Enemy(std::pair<float, float> pos_, std::pair<float, float> dest_, Map& map);
    void findPath(int fromRow, int fromCol, int toRow, int toCol, std::vector<std::pair<int, int>>& p, Map& map);
    void setPosition(std::pair<float, float> pos_, std::pair<float, float> dest_, Map& map);
    void update(float time, Map& map);
    void updateActionAndDir(Map& map);
    Direction getDirection() const;
    std::pair<float, float> getPos() const;
    sf::Sprite sprite;
    float currFrame = 0;
    const float w = 25.0, h = 41.75;
};
#endif