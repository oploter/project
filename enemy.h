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
    float force = 5;
    float timeNextAttack = 1.5;
    bool alive = true;
    std::pair<float, float> pos;
    std::pair<float, float> dest;
    std::pair<int, int> mapPos;
    std::vector<std::pair<int, int>> path;
    std::vector<std::vector<std::pair<int, bool>>> mem;
    Direction dir = Direction::DOWN;
    Action action = Action::ENTRY;
    void updateMapPos();

  public:
    Enemy(){}
    Enemy(Map &map, int id_);
    virtual ~Enemy() = default;
    void findPath(int fromRow, int fromCol, int toRow, int toCol,
                  std::vector<std::pair<int, int>> &p, Map &map);
    void setPosition(std::pair<float, float> pos_,
                     std::pair<float, float> dest_, Map &map);
    virtual void update(float time, Model &model) = 0;
    virtual void updateActionAndDir(Map &map) = 0;
    Direction getDirection() const;
    Action getAction() const;
    std::pair<float, float> getPos() const;
    std::pair<float, float> getDest() const;
    std::pair<int, int> getMapPos() const;
    int hp = 100;
    int id;
    float currFrame = 0;
    const float w = 25.0, h = 41.75;
    const float speed = BlockSize / 6;
    bool isAlive() const;
};
struct Zombie : Enemy {
    Zombie(Map &map, int id_);
    Zombie(std::pair<float, float> pos_, std::pair<float, float> dest_, Map& map);
    void update(float time, Model &model) override;
    void updateActionAndDir(Map &map) override;
};
struct ClientEnemy{
    int id;
    std::pair<float, float> pos;
    int hp;
    Direction dir;
    Action action;
    float currFrame = 0;
};
#endif
