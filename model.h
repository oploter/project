#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "map.h"
#include "player.h"
#include "vars.h"
#include <string>
#include <vector>
struct Model {
private:
  Map map;
  std::vector<Button> buttons;
  Player plr;

public:
  GameState state;
  Map &getMap();
  Player &getPlayer();
  void update(float time);
  const std::vector<Button> &getButtons() const;
  Model(const std::string &mapFileName);
};
#endif