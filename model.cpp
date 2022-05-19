#include "model.h"
#include "map.h"
#include <iostream>
#include <string>
const Map &Model::getMap() const { return map; }
Model::Model(const std::string &mapFileName) : map(mapFileName) {}
Player::Player(int id_) : id(id_), x(0), y(0), speed(0.5), dir(PlayerDir::STILL){}