#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "map.h"
#include "player.h"
#include "enemy.h"
#include "vars.h"
#include <string>
#include <vector>
struct Model {
private:
  Map map;
  std::vector<Button> buttons;
  Player plr;
  std::vector<Enemy> enemies;

public:
  Model(const std::string &mapFileName, int numOfEnemies = 1);
  GameState state;
  Map &getMap();
  Player &getPlayer();
  void update(float time);
  const std::vector<Button> &getButtons() const;
  std::vector<Enemy>& getEnemies();
  std::pair<float, float> randPoint(bool isGreen = false);
};
#endif