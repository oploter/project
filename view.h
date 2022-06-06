#ifndef VIEW_H
#define VIEW_H
#include "model.h"
#include "vars.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
struct View {
private:
  sf::RenderWindow &window;
  sf::Sprite mapSprite;
  sf::Sprite enemySprite;
  Model *model;
  int screen_width, screen_height;
  sf::Text text_p, text_m, text_h, text_s, text_ph, text_bul;
  std::string currPlrName = "";

public:
  static sf::Font *get_or_create_font(const std::string &font_name,
                                      const std::string &path_to_font = "");
  static sf::Texture *
  get_or_create_texture(const std::string &texture_name,
                        const std::string &path_to_texture = "",
                        bool transparent = false);
  View(sf::RenderWindow &window_, Model &model_);
  void drawMap(Player& thisPlr);
  void drawMenu();
  void drawEnemies(std::map<int, ClientEnemy>& enemies);
  void changeModel(Model &new_model);
  void setName(const std::string& tmpName);
};
#endif