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
#include <sstream>
#include <cassert>
sf::Font *View::get_or_create_font(const std::string &font_name,
                                   const std::string &path_to_font) {
    static std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    if (fonts.count(font_name) == 0) {
        fonts[font_name] = std::make_unique<sf::Font>();
        if (!fonts[font_name]->loadFromFile(path_to_font)) {
            throw std::runtime_error("Could not load font " + path_to_font);
        }
    }
    return fonts[font_name].get();
}
sf::Texture *View::get_or_create_texture(const std::string &texture_name,
                                         const std::string &path_to_texture,
                                         bool transparent) {
    static std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    if (textures.count(texture_name) == 0) {
        sf::Image img;
        img.loadFromFile(path_to_texture);
        if (transparent) {
            img.createMaskFromColor(img.getPixel(0, 0));
        }
        textures[texture_name] = std::make_unique<sf::Texture>();
        if (path_to_texture != "" && !textures[texture_name]->loadFromImage(img)) {
            throw std::runtime_error("Could not load " + path_to_texture);
        }
    }
    return textures[texture_name].get();
}
View::View(sf::RenderWindow &window_, Model &model_)
        : window(window_), model(&model_), screen_width(window.getSize().x),
          screen_height(window.getSize().y) {
    get_or_create_texture("map", "img/blocks.png");
    get_or_create_font("font", "img/arial.ttf");
    get_or_create_texture("bullet", "img/bullet.png");
    enemySprite.setTexture(*get_or_create_texture("enemy", "img/enemy.png", true));

    text_s.setFont(*get_or_create_font("font"));
    text_s.setCharacterSize(20);
    text_s.setFillColor(sf::Color::Yellow);
    text_s.setStyle(sf::Text::Bold);

    text_h.setFont(*get_or_create_font("font"));
    text_h.setCharacterSize(20);
    text_h.setFillColor(sf::Color::Green);
    text_h.setStyle(sf::Text::Bold);

    text_m.setFont(*get_or_create_font("font"));
    text_m.setCharacterSize(20);
    text_m.setFillColor(sf::Color::Red);
    text_m.setStyle(sf::Text::Bold);

    text_p.setFont(*get_or_create_font("font"));
    text_p.setCharacterSize(20);
    text_p.setFillColor(sf::Color::Black);
    text_p.setStyle(sf::Text::Bold);

    text_ph.setFont(*get_or_create_font("font"));
    text_ph.setCharacterSize(20);
    text_ph.setFillColor(sf::Color::Red);
    text_ph.setStyle(sf::Text::Bold);

    text_bul.setFont(*get_or_create_font("font"));
    text_bul.setCharacterSize(20);
    text_bul.setFillColor(sf::Color::Black);
    text_bul.setStyle(sf::Text::Bold);
}
void View::drawMap(Player& thisPlr) {
    mapSprite.setTexture(*get_or_create_texture("map"));
    for (int col = 0; col < model->getMap().getCols(); col++) {
        for (int row = 0; row < model->getMap().getRows(); row++) {
            mapSprite.setPosition(col * BlockSize, row * BlockSize);
            BlockType t = model->getMap().at(row, col);
            if(model->getMap().time_hurt[row][col] > 0 && (t == BlockType::SMALL_FLOWER || t == BlockType::AVERAGE_FLOWER || t == BlockType::BIG_FLOWER)){
                mapSprite.setColor(sf::Color::Red);
            }else if(model->getMap().time_hurt[row][col] > 0 && (t == BlockType::GREEN)){
                model->getMap().time_hurt[row][col] = 0;
            }
            switch (t) {
                case BlockType::WATER:
                    mapSprite.setTextureRect(sf::IntRect(100, 0, BlockSize, BlockSize));
                    break;
                case BlockType::GREEN:
                    if (model->getMap().score_of_coins[row][col] == 0) mapSprite.setTextureRect(sf::IntRect(50, 0, BlockSize, BlockSize));
                    else {
                        mapSprite.setTextureRect(sf::IntRect(150, 0, BlockSize, BlockSize));
                    }
                    break;
                case BlockType::CONCRETE:
                    mapSprite.setTextureRect(sf::IntRect(0, 0, BlockSize, BlockSize));
                    break;
                case BlockType::MOLE:
                    mapSprite.setTextureRect(sf::IntRect(50, 0, BlockSize, BlockSize));
                    break;
                case BlockType::SMALL_FLOWER:
                    if (model->getMap().score_of_coins[row][col] == 0) {
                        mapSprite.setTextureRect(sf::IntRect(200, 0, BlockSize, BlockSize));
                    }
                    else {
                        mapSprite.setTextureRect(sf::IntRect(350, 0, BlockSize, BlockSize));
                    }
                    break;
                case BlockType::AVERAGE_FLOWER:
                    if (model->getMap().score_of_coins[row][col] == 0) {
                        mapSprite.setTextureRect(sf::IntRect(250, 0, BlockSize, BlockSize));
                    }
                    else {
                        mapSprite.setTextureRect(sf::IntRect(400, 0, BlockSize, BlockSize));
                    }
                    break;
                case BlockType::BIG_FLOWER:
                    if (model->getMap().score_of_coins[row][col] == 0) {
                        mapSprite.setTextureRect(sf::IntRect(300, 0, BlockSize, BlockSize));
                    }
                    else {
                        mapSprite.setTextureRect(sf::IntRect(450, 0, BlockSize, BlockSize));
                    }
                    break;
            }
            window.draw(mapSprite);
            mapSprite.setColor(sf::Color::White);
        }
    }
    std::wostringstream PlayerScore;
    PlayerScore << thisPlr.playerscore;
    text_s.setString(L"Собрано монет:" + PlayerScore.str());
    text_s.setPosition(0, 0);
    window.draw(text_s);

    std::wostringstream PlayerHealth;
    PlayerHealth << thisPlr.health;
    text_h.setString(L"Здоровье:" + PlayerHealth.str());
    text_h.setPosition(0, 700);
    window.draw(text_h);

    std::wostringstream PlayerBullets;
    PlayerBullets << thisPlr.bullets;
    text_bul.setString(L"Количество патронов:" + PlayerBullets.str());
    text_bul.setPosition(450, 700);
    window.draw(text_bul);

    std::vector<int> drawing;
    if(model->type == 2){
        for(auto it = static_cast<ClientModel*>(model)->getBullets().begin(); it != static_cast<ClientModel*>(model)->getBullets().end(); it++){
            if(it->second.life == false){
                continue;
            }
            it->second.sprite.setPosition(it->second.x, it->second.y);
            it->second.sprite.setTexture(*get_or_create_texture("bullet"));
            drawing.push_back(it->second.id);
            window.draw(it->second.sprite);
        }
    }
    static_cast<ClientModel*>(model)->getBullets().clear();
    for (auto& u : model->getPlayers()) {
        if (model->getNames()[u.first] == true) {
            sf::Text text_p("", *(View::get_or_create_font("font")), 20);
            sf::Text text_ph("", *(View::get_or_create_font("font")), 20);
            //std::wstring widestr = std::wstring(u.first.begin(), u.first.end());
            text_p.setString(u.first);
            text_p.setPosition(model->getPlayers().at(u.first).x, model->getPlayers().at(u.first).y - 20);
            text_ph.setString(u.first);
            text_ph.setPosition(model->getPlayers().at(u.first).x, model->getPlayers().at(u.first).y - 20);
            if (u.first == currPlrName) {
                window.draw(text_ph);
            } else {
                window.draw(text_p);
            }
            window.draw(model->getPlayers().at(u.first).sprite);
            //std::cout << players.at(u.first).x << " " << players.at(u.first).y << "\n";
        }
    }
    //assert(static_cast<ClientModel*>(model)->getEnemies().size() > 0);
    drawEnemies(static_cast<ClientModel*>(model)->getEnemies());
}
void View::drawMenu() {
    window.clear(sf::Color(100, 100, 100));
    for (const Button &button : model->getButtons()) {
        sf::Text sprite(button.getLabel(), *get_or_create_font("font"), 30);
        sprite.setPosition(button.coordinates().first, button.coordinates().second);
        window.draw(sprite);
    }
    if(model->state == GameState::DIED){
        text_m.setString(L"Вы умерли");
        text_m.setPosition(250, 300);
        text_m.setFillColor(sf::Color::Red);
        window.draw(text_m);
    }
}
void View::drawEnemies(std::map<int, ClientEnemy>& enemies){
    for(auto it = enemies.begin(); it != enemies.end(); it++){
        ClientEnemy& enemy = it->second;
        switch (enemy.dir)
        {
        case Direction::LEFT:
            enemySprite.setTextureRect(sf::IntRect(25 * int(enemy.currFrame), 84, 25, 42));
            break;
        case Direction::RIGHT:
            enemySprite.setTextureRect(sf::IntRect(25 * int(enemy.currFrame), 42, 25, 42));
            break;  
        case Direction::DOWN:
            enemySprite.setTextureRect(sf::IntRect(25 * int(enemy.currFrame), 125, 25, 42));
            break;
        case Direction::UP:
            enemySprite.setTextureRect(sf::IntRect(25 * int(enemy.currFrame), 0, 25, 42));
            break;
        case Direction::STOP:
            enemySprite.setTextureRect(sf::IntRect(0, 125, 25, 42));
            break;
        default:
            break;
        }   
        enemySprite.setPosition(enemy.pos.second, enemy.pos.first);
        window.draw(enemySprite);
        sf::RectangleShape hpLine(sf::Vector2f((static_cast<float>(enemy.hp) / 100) * IMG_W, 5));
        hpLine.setFillColor(sf::Color::Green);
        hpLine.setPosition(enemy.pos.second, enemy.pos.first - 5);
        window.draw(hpLine);
    }
}
void View::changeModel(Model &new_model){
    model = &new_model;
}
void View::setName(const std::string& tmpName){
    currPlrName = tmpName;
}