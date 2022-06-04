#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "enemy.h"
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
    std::vector<std::unique_ptr<Enemy>> enemies;

  public:
    GameState state;
    Map &getMap();
    Player &getPlayer();
    void update(float time);
    const std::vector<Button> &getButtons() const;
    Model(const std::string &mapFileName, int numOfEnemies = 1);
    std::vector<std::unique_ptr<Enemy>> &getEnemies();
    std::pair<float, float> randPoint(bool isGreen = false);
};
#endif