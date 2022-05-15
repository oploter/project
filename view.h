#ifndef VIEW_H
#define VIEW_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
struct View{
private:
    int screen_width, screen_height;
    sf::RenderWindow& window;
    int mapBlockSize = 50;
    // Model& model;
    sf::Texture *get_or_create_texture(const std::string& texture_name, const std::string& path_to_texture = "");
    sf::Font *get_or_create_font(const std::string& font_name, const std::string& path_to_font = "");
public:
    View(sf::RenderWindow& window_);
    void drawMap();
};
#endif