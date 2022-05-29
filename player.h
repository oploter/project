#ifndef PLAYER_H
#define PLAYER_H
#include "vars.h"
#include <SFML/Graphics.hpp>

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
struct Model;
class Player { // класс Игрока
public:
    float x, y, w, h, dx, dy, speed, health, ability, ind; //координаты игрока х и у, высота ширина, ускорение (по х и по у), сама скорость
    bool life;
    int dir, playerscore; //направление (direction) движения игрока
    float leftOnWater = 3; // можно на воде простоять 4 секунды
    sf::String File; //файл с расширением
    sf::Image image;//сфмл изображение
    sf::Texture texture;//сфмл текстура
    sf::Sprite sprite;//сфмл спрайт
    Player(sf::String F, float X, float Y, float W, float H);
    void update(float time, Model &model);
    void interactionWithMap(float time, Model &model);

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }
};
#endif