#ifndef MODEL_H
#define MODEL_H
#include "map.h"
struct Model {
  private:
    Map map;

  public:
    const Map &getMap() const;
    Model(const std::string &mapFileName);
};
struct Player {
    int id;
    float x, y, speed;
    PlayerDir dir;
    Player(int id_);
};
#endif