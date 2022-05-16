#ifndef VIEW_H
#define VIEW_H
#include "model.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
struct View {
private:
  sf::RenderWindow &window;
  sf::Sprite mapSprite;
  Model &model;
  int screen_width, screen_height;
  sf::Texture *get_or_create_texture(const std::string &texture_name,
                                     const std::string &path_to_texture = "");
  sf::Font *get_or_create_font(const std::string &font_name,
                               const std::string &path_to_font = "");

public:
  View(sf::RenderWindow &window_, Model &model_);
  void drawMap();
};
#endif