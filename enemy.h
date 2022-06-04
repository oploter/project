#ifndef ENEMY_H
#define ENEMY_H
#include "map.h"
#include "vars.h"
#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>
struct Model;
struct Enemy {
  protected:
    float speed = BlockSize / 4;
    float force = 6;
    float timeNextAttack = 1.5;
    std::pair<float, float> pos;
    std::pair<int, int> mapPos;
    std::vector<std::pair<int, int>> path;
    std::vector<std::vector<std::pair<int, bool>>> mem;
    Direction dir = Direction::DOWN;
    Action action = Action::ENTRY;

  public:
    Enemy(Map &map);
    void findPath(int fromRow, int fromCol, int toRow, int toCol,
                  std::vector<std::pair<int, int>> &p, Map &map);
    void setPosition(std::pair<float, float> pos_,
                     std::pair<float, float> dest_, Map &map);
    virtual void update(float time, Model &model) = 0;
    virtual void updateActionAndDir(Map &map) = 0;
    Direction getDirection() const;
    std::pair<float, float> getPos() const;
    sf::Sprite sprite;
    float currFrame = 0;
    int hp = 100;
    const float w = 25.0, h = 41.75;
};
struct Zombie : Enemy {
    Zombie(Map &map);
    void update(float time, Model &model) override;
    void updateActionAndDir(Map &map) override;
};
#endif