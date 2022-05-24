#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "map.h"
#include "vars.h"
#include <string>
#include <vector>
struct Model {
private:
  Map map;
  std::vector<Button> buttons;

public:
  Map &getMap();
  void update(float time);
  const std::vector<Button> &getButtons() const;
  Model(const std::string &mapFileName);
};
#endif