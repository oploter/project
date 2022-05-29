#include "player.h"
#include "model.h"
#include <cassert>
#include <SFML/Graphics.hpp>
#include <iostream>

Player::Player(sf::String F, float X, float Y, float W, float H){
    playerscore = 0;
    health = 100;
    ability = 50;
    bul_ability = 50;
    ind = 0;
    life = true;
    dx=0;dy=0;speed=0;dir=0;
    File = F;
    w = W; h = H;
    image.loadFromFile("../img/" + File);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    x = X; y = Y;
    sprite.setTextureRect(sf::IntRect(0, 0, w, h));
}
void Player::update(float time, Model& model)
{
    switch (dir)
    {
        case 0: dx = speed; dy = 0; break;
        case 1: dx = -speed; dy = 0; break;
        case 2: dx = 0; dy = speed; break;
        case 3: dx = 0; dy = -speed; break;
    }
    Map &map = model.getMap();
    switch(dir){
        case 0:
        {
            dx = speed, dy = 0;
            if(map.at((y + h / 2) / BlockSize, (x + w) / BlockSize) == BlockType::CONCRETE){
                break;
            }
            x += dx * time;
        }
        break;
        case 1:
        {
            dx = -speed, dy = 0;
            if(map.at((y + h / 2) / BlockSize, x / BlockSize) == BlockType::CONCRETE){
                break;
            }
            x += dx * time;
        }
        break;
        case 2:
        {
            dx = 0, dy = speed;
            if(map.at((y + h) / BlockSize,  (x + w / 2)/ BlockSize) == BlockType::CONCRETE){
                break;
            }
            y += dy * time;
        }
        break;
        case 3:
        {
            dx = 0, dy = -speed;
            if(map.at((y) / BlockSize, (x + w / 2) / BlockSize) == BlockType::CONCRETE){
                break;
            }
            y += dy * time;
        }
        break;
    }
    //x += dx*time;//то движение из прошлого урока. наше ускорение на время получаем смещение координат и как следствие движение
    //y += dy*time;//аналогично по игреку

    speed = 0;
    sprite.setPosition(x,y);
    interactionWithMap(time, model);
}
void Player::interactionWithMap(float time, Model &model) {
    for (int i = y / 50; i < (y + h) / 50; i++) {
        for (int j = x / 50; j < (x + w) / 50; j++) {
            if(i >= model.getMap().getRows() || j >= model.getMap().getCols()){
                continue;
            }
            if (model.getMap().field[i][j] == BlockType::WATER) {
                leftOnWater -= time / 2'000;
            }
            if (model.getMap().field[i][j] == BlockType::MOLE) {
                health -= 0.001;
            }
            playerscore += model.getMap().score_of_coins[i][j];
            model.getMap().score_of_coins[i][j] = 0;
        }
    }
}