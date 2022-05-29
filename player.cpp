#include "player.h"
#include "model.h"
#include <cassert>
#include <SFML/Graphics.hpp>
#include <iostream>

Player::Player(sf::String F, float X, float Y, float W, float H){ //Конструктор с параметрами(формальными) для класса Player. При создании объекта класса мы будем задавать имя файла, координату Х и У, ширину и высоту
    playerscore = 0;
    health = 100;
    ability = 50;
    ind = 0;
    life = true;
    dx=0;dy=0;speed=0;dir=0;
    File = F;//имя файла+расширение
    w = W; h = H;//высота и ширина
    image.loadFromFile("../img/" + File);//запихиваем в image наше изображение вместо File мы передадим то, что пропишем при создании объекта. В нашем случае "hero.png" и получится запись идентичная image.loadFromFile("images/hero/png");
    //image.createMaskFromColor(sf::Color(255, 192, 203));//убираем ненужный темно-синий цвет, эта тень мне показалась не красивой.
    texture.loadFromImage(image);//закидываем наше изображение в текстуру
    sprite.setTexture(texture);//заливаем спрайт текстурой
    x = X; y = Y;//координата появления спрайта
    sprite.setTextureRect(sf::IntRect(0, 0, w, h)); //Задаем спрайту один прямоугольник для вывода одного льва, а не кучи львов сразу. IntRect - приведение типов
}
void Player::update(float time, Model& model) //функция "оживления" объекта класса. update - обновление. принимает в себя время SFML , вследствие чего работает бесконечно, давая персонажу движение.
{
    switch (dir)//реализуем поведение в зависимости от направления. (каждая цифра соответствует направлению)
    {
        case 0: dx = speed; dy = 0; break;//по иксу задаем положительную скорость, по игреку зануляем. получаем, что персонаж идет только вправо
        case 1: dx = -speed; dy = 0; break;//по иксу задаем отрицательную скорость, по игреку зануляем. получается, что персонаж идет только влево
        case 2: dx = 0; dy = speed; break;//по иксу задаем нулевое значение, по игреку положительное. получается, что персонаж идет только вниз
        case 3: dx = 0; dy = -speed; break;//по иксу задаем нулевое значение, по игреку отрицательное. получается, что персонаж идет только вверх
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

    speed = 0;//зануляем скорость, чтобы персонаж остановился.
    sprite.setPosition(x,y); //выводим спрайт в позицию x y , посередине. бесконечно выводим в этой функции, иначе бы наш спрайт стоял на месте.
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