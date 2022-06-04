#include "view.h"
#include "button.h"
#include "map.h"
#include "vars.h"
#include <SFML/Graphics.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
bool checkIsPlant(BlockType t);
sf::Font *View::get_or_create_font(const std::string &font_name,
                                   const std::string &path_to_font) {
    static std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    if (fonts.count(font_name) == 0) {
        fonts[font_name] = std::make_unique<sf::Font>();
        if (!fonts[font_name]->loadFromFile("../img/" + path_to_font)) {
            throw std::runtime_error("Could not load font " + path_to_font);
        }
    }
    return fonts[font_name].get();
}
sf::Texture *View::get_or_create_texture(const std::string &texture_name,
                                         const std::string &path_to_texture,
                                         bool transparent) {
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>>
        textures;
    if (textures.count(texture_name) == 0) {
        sf::Image img;
        img.loadFromFile("../img/" + path_to_texture);
        if (transparent) {
            img.createMaskFromColor(img.getPixel(0, 0));
        }
        textures[texture_name] = std::make_unique<sf::Texture>();
        if (path_to_texture != "" &&
            !textures[texture_name]->loadFromImage(img)) {
            throw std::runtime_error("Could not load " + path_to_texture);
        }
    }
    return textures[texture_name].get();
}
View::View(sf::RenderWindow &window_, Model &model_)
    : window(window_), model(&model_), screen_width(window.getSize().x),
      screen_height(window.getSize().y) {
    get_or_create_texture("map", "blocks.png");
    get_or_create_font("font", "arial.ttf");
}
void View::drawMap() {
    [[maybe_unused]] const Map &map = model->getMap();
    [[maybe_unused]] auto &mapTexture = *get_or_create_texture("map");
    mapSprite.setTexture(mapTexture);
    for (int col = 0; col < map.getCols(); col++) {
        for (int row = 0; row < map.getRows(); row++) {
            mapSprite.setPosition(col * BlockSize, row * BlockSize);
            if (map.time_hurt[row][col] > 0 && checkIsPlant(map.at(row, col))) {
                mapSprite.setColor(sf::Color::Red);
            }
            switch (map.at(row, col)) {
            case BlockType::WATER:
                mapSprite.setTextureRect(
                    sf::IntRect(100, 0, BlockSize, BlockSize));
                break;
            case BlockType::GREEN:
                if (map.score_of_coins[row][col] == 0)
                    mapSprite.setTextureRect(
                        sf::IntRect(50, 0, BlockSize, BlockSize));
                else {
                    mapSprite.setTextureRect(
                        sf::IntRect(150, 0, BlockSize, BlockSize));
                }
                break;
            case BlockType::CONCRETE:
                mapSprite.setTextureRect(
                    sf::IntRect(0, 0, BlockSize, BlockSize));
                break;
            case BlockType::MOLE:
                mapSprite.setTextureRect(
                    sf::IntRect(50, 0, BlockSize, BlockSize));
                break;
            case BlockType::SMALL_FLOWER:
                if (map.score_of_coins[row][col] == 0) {
                    mapSprite.setTextureRect(
                        sf::IntRect(200, 0, BlockSize, BlockSize));
                } else {
                    mapSprite.setTextureRect(
                        sf::IntRect(350, 0, BlockSize, BlockSize));
                }
                break;
            case BlockType::AVERAGE_FLOWER:
                if (map.score_of_coins[row][col] == 0) {
                    mapSprite.setTextureRect(
                        sf::IntRect(250, 0, BlockSize, BlockSize));
                } else {
                    mapSprite.setTextureRect(
                        sf::IntRect(400, 0, BlockSize, BlockSize));
                }
                break;
            case BlockType::BIG_FLOWER:
                if (map.score_of_coins[row][col] == 0) {
                    mapSprite.setTextureRect(
                        sf::IntRect(300, 0, BlockSize, BlockSize));
                } else {
                    mapSprite.setTextureRect(
                        sf::IntRect(450, 0, BlockSize, BlockSize));
                }
                break;
            }
            window.draw(mapSprite);
            mapSprite.setColor(sf::Color::White);
        }
    }
}
void View::drawMenu() {
    window.clear(sf::Color(100, 100, 100));
    for (const Button &button : model->getButtons()) {
        sf::Text sprite(button.getLabel(), *get_or_create_font("font"), 30);
        sprite.setPosition(button.coordinates().first,
                           button.coordinates().second);
        window.draw(sprite);
    }
}
void View::drawEnemy(Enemy &enemy, float time) {
    switch (enemy.getDirection()) {
    case Direction::DOWN:
        enemy.sprite.setTextureRect(
            sf::IntRect(25 * static_cast<int>(enemy.currFrame), 125, 25, 42));
        break;
    case Direction::UP:
        enemy.sprite.setTextureRect(
            sf::IntRect(25 * static_cast<int>(enemy.currFrame), 0, 25, 42));
        break;
    case Direction::LEFT:
        enemy.sprite.setTextureRect(
            sf::IntRect(25 * static_cast<int>(enemy.currFrame), 84, 25, 42));
        break;
    case Direction::RIGHT:
        enemy.sprite.setTextureRect(
            sf::IntRect(25 * static_cast<int>(enemy.currFrame), 42, 25, 42));
        break;
    case Direction::STOP:
        enemy.sprite.setTextureRect(sf::IntRect(0, 125, 25, 42));
        break;
    }
    enemy.currFrame += time * 5;
    if (enemy.currFrame >= 4) {
        enemy.currFrame = 0;
    }
    enemy.sprite.setPosition(enemy.getPos().second, enemy.getPos().first);
    window.draw(enemy.sprite);
    sf::RectangleShape hpLeftLine(sf::Vector2f(enemy.w * enemy.hp / 100, 5));
    hpLeftLine.setFillColor(sf::Color::Green);
    hpLeftLine.setPosition(enemy.getPos().second, enemy.getPos().first - 5);
    window.draw(hpLeftLine);
}
void View::changeModel(Model &new_model) { model = &new_model; }