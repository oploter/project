#include "view.h"
#include "map.h"
#include "vars.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
sf::Font *View::get_or_create_font(const std::string& font_name, const std::string& path_to_font){
    static std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    if(fonts.count(font_name) == 0){
        fonts[font_name] = std::make_unique<sf::Font>();
        if(!fonts[font_name]->loadFromFile(path_to_font)){
            throw std::runtime_error("Could not load font " + path_to_font);
        }
    }
    return fonts[font_name].get(); 
}
sf::Texture *View::get_or_create_texture(const std::string& texture_name, const std::string& path_to_texture){
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    if(textures.count(texture_name) == 0){
        std::cout << "creating texture " << texture_name << std::endl;
        textures[texture_name] = std::make_unique<sf::Texture>();
        if(path_to_texture != "" && !textures[texture_name]->loadFromFile(path_to_texture)){
            throw std::runtime_error("Could not load " + path_to_texture);
        }
    }
    return textures[texture_name].get();
}
View::View(sf::RenderWindow& window_) : window(window_), screen_width(window.getSize().x), screen_height(window.getSize().y){
    for(const std::string& name : texturesToLoad){
        get_or_create_texture(name, "img/" + name + ".png");
    }
    for(const std::string& name : fontsToLoad){
        get_or_create_font(name, "img/" + name + ".otf");
    }
}
void View::drawMap(){
    Map& map = model.getMap();
    for(int row = 0; row < map.getRows(); row++){
        for(int col = 0; col < map.getCols(); col++){
            sf::Sprite block;
            block.setTexture(*get_or_create_texture(blockTotexture.at(map.at(row, col))));
            block.setPosition(row * mapBlockSize, col * mapBlockSize);
            window.draw(block);
        }
    }
}