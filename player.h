#ifndef PLAYER_H
#define PLAYER_H
#include "vars.h"
#include <SFML/Graphics.hpp>

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
struct Model;
class Player {
public:
    float x, y, w, h, dx, dy, speed, health, ability, bul_ability, ind;
    bool life;
    int dir, playerscore;
    float leftOnWater = 3;
    float CurrentFrame = 0;
    sf::String File;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    Player(sf::String F, float X, float Y, float W, float H);
    void update(float time, Model &model);
    void interactionWithMap(float time, Model &model);
};
#endif