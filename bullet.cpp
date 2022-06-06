#include "bullet.h"
#include "model.h"
#include "player.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

Bullet::Bullet(sf::String F, float X, float Y, float W, float H, int dir_){
    dx=0;dy=0;speed=0.15;dir=dir_;
    life = true;
    File = F;
    w = W; h = H;
    image.loadFromFile("../img/" + File);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    x = X; y = Y;//координата появления спрайта
    sprite.setTextureRect(sf::IntRect(0, 0, w, h));
    sprite.setPosition(x, y);
}
ClientBullet::ClientBullet(sf::String F, float X, float Y, float W, float H, int dir_) : Bullet(F, X, Y, W, H, dir_){}
ServerBullet::ServerBullet(sf::String F, float X, float Y, float W, float H, int dir_, const std::string &name_) : Bullet(F, X, Y, W, H, dir_){
    name = name_;
}
void ServerBullet::update(float time, ServerModel &model){
    switch (dir)
    {
        case 0: dx = speed; dy = 0; break;
        case 1: dx = -speed; dy = 0; break;
        case 2: dx = 0; dy = speed; break;
        case 3: dx = 0; dy = -speed; break;
    }

    x += dx*time;
    y += dy*time;

    sprite.setPosition(x,y);
    interactionWithMap(model);
}
void ServerBullet::interactionWithMap(ServerModel& model){
    for (int i = y / 50; i < (y + h) / 50; i++) {
        for (int j = x / 50; j < (x + w) / 50; j++) {
            if (i <= 0 || j <= 0 || i >= (model.getMap().getRows() - 1) || j >= (model.getMap().getCols() - 1)) {
                life = false;
            }
            else if (model.getMap().field[i][j] == BlockType::SMALL_FLOWER || model.getMap().field[i][j] == BlockType::AVERAGE_FLOWER || model.getMap().field[i][j] == BlockType::BIG_FLOWER) {
                life = false;
                model.getMap().life_of_flowers[i][j] -= 20;
            }
            else if (model.getMap().map_of_players[i][j] != "") {
                std::string player_name = model.getMap().map_of_players[i][j];
                if (player_name != name) {
                    model.getPlayers().at(player_name).health -= 20;
                    life = false;
                }
            }
        }
    }
    std::cout << "before game enemy upd\n";
    for(auto& enemy : model.getEnemies()){
        if(enemy->getPos().first <= y && y <= enemy->getPos().first + IMG_H && enemy->getPos().second <= x && x <= enemy->getPos().second + IMG_W){
            enemy->hp -= force;
            life = false;
            std::cout << "KILL " << enemy->hp << "\n";
            break;
        }
    }
    std::cout << "after upd\n";
}