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
#endif