#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "map.h"
#include "vars.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <utility>
#include <vector>
struct Model {
private:
  Map map;
  std::vector<Button> buttons;

public:
  const Map &getMap() const;
  const std::vector<Button> &getButtons() const;
  Model(const std::string &mapFileName);
};
#endif