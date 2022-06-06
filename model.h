#ifndef MODEL_H
#define MODEL_H
#include "button.h"
#include "map.h"
#include "player.h"
#include "vars.h"
#include "enemy.h"
#include "bullet.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
struct Model {
protected:
  Map map;
  std::vector<Button> buttons;
  Player plr;
  std::map <std::string, Player> players;
  std::map <std::string, bool> names;

public:
  virtual ~Model() = default;
  GameState state;
  Map &getMap();
  Player &getPlayer();
  void update(float time);
  const std::vector<Button> &getButtons() const;
  std::map<std::string, Player>& getPlayers();
  std::map<std::string, bool>& getNames();
  Model(const std::string &mapFileName);
  std::pair<float, float> randPoint(bool isGreen = false);
  int type;
};
struct ServerModel : Model{
private:
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<ServerBullet>> bullets;
public:
    ServerModel(const std::string& mapFileName, int numofEnemies = 1);
    std::vector<std::unique_ptr<Enemy>>& getEnemies();
    std::vector<std::unique_ptr<ServerBullet>>& getBullets();
    void update(float time);
};
struct ClientModel : Model{
private:
    std::map<int, ClientEnemy> enemies;
    std::vector<std::unique_ptr<ClientBullet>> bullets;
public:
    std::map<int, ClientEnemy>& getEnemies();
    std::vector<std::unique_ptr<ClientBullet>>& getBullets();
    ClientModel(const std::string& mapFileName);
};
#endif