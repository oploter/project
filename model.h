#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "map.h"
#include "player.h"
#include "vars.h"
#include "enemy.h"
#include <string>
#include <memory>
#include <vector>
struct Model {
protected:
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
  std::pair<float, float> randPoint(bool isGreen = false);
};
struct ServerModel : Model{
private:
    std::vector<std::unique_ptr<Enemy>> enemies;
public:
    ServerModel(const std::string& mapFileName, int numofEnemies = 1);
    std::vector<std::unique_ptr<Enemy>>& getEnemies();
    void update(float time);
};
struct ClientModel : Model{
public:
    ClientModel(const std::string& mapFileName);
};
#endif