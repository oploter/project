#include "model.h"
#include "map.h"
#include <iostream>
#include <string>
Map &Model::getMap() { return map; }
const std::vector<Button> &Model::getButtons() const { return buttons; }
Model::Model(const std::string &mapFileName) : map(mapFileName) {
  buttons.emplace_back("Single game", 250, 350);
  buttons.emplace_back("Exit", 250, 380);
}