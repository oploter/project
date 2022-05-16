#include "model.h"
#include "map.h"
#include <iostream>
#include <string>
const Map &Model::getMap() const { return map; }
Model::Model(const std::string &mapFileName) : map(mapFileName) {}