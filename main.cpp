#include "model.h"
#include "vars.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <deque>
#include <utility>

class Bullet {
private:
    float x, y;
public:
    float w, h, dx, dy, speed;
    bool life;
    int dir;
    sf::String File;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    Bullet(sf::String F, float X, float Y, float W, float H, int dir_){
        dx=0;dy=0;speed=0.15;dir=dir_;
        File = F;
        w = W; h = H;
        image.loadFromFile("../img/" + File);
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        x = X; y = Y;//координата появления спрайта
        sprite.setTextureRect(sf::IntRect(0, 0, w, h));
        sprite.setPosition(x, y);
    }

    void update(float time, Model &model)
    {
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

    void interactionWithMap(Model &model) {
        for (int i = y / 50; i < (y + h) / 50; i++) {
            for (int j = x / 50; j < (x + w) / 50; j++) {
                if (i < 0 || j < 0 || i > model.getMap().getRows() || j > model.getMap().getCols()) {
                    life = false;
                }
                else if (model.getMap().field[i][j] == BlockType::SMALL_FLOWER || model.getMap().field[i][j] == BlockType::AVERAGE_FLOWER || model.getMap().field[i][j] == BlockType::BIG_FLOWER) {
                    life = false;
                    model.getMap().life_of_flowers[i][j] -= 20;
                }
            }
        }
    }

    float getX() {
        return x;
    }

    float getY() {
        return y;
    }
};

std::deque <std::unique_ptr<Bullet>> bullets;

int main() {
    sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
    Model model("../map.txt");
    View view(window, model);

    Player& p = model.getPlayer();//создаем объект p класса player,задаем "hero.png" как имя файла+расширение, далее координата Х,У, ширина, высота.

    float CurrentFrame = 0;//хранит текущий кадр
    sf::Clock clock;

    sf::Font score;
    score.loadFromFile("../arial.ttf");
    sf::Text text_s("", score, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_s.setFillColor(sf::Color::Black);
    text_s.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Font Health;
    Health.loadFromFile("../arial.ttf");
    sf::Text text_h("", Health, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_h.setFillColor(sf::Color::Black);
    text_h.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Font Message;
    Message.loadFromFile("../arial.ttf");
    sf::Text text_m("", Message, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_m.setFillColor(sf::Color::Red);
    text_m.setStyle(sf::Text::Bold | sf::Text::Underlined);

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time = time / 500;
        sf::Event ev;

        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }
            if(model.state == GameState::MENU || model.state == GameState::DIED){
                if(ev.type == sf::Event::MouseButtonPressed){
                    auto mousePos = sf::Mouse::getPosition(window);
                    if(model.getButtons()[0].isClicked(mousePos.x, mousePos.y)){ // нажали "Single game"
                        model.state = GameState::GAME;
                    }else if(model.getButtons()[1].isClicked(mousePos.x, mousePos.y)){ // нажали "Exit"
                        window.close();
                    }
                }
                break;
            }
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && p.getX() > 0) { //если нажата клавиша стрелка влево или англ буква А
            p.dir = 1; p.speed = 0.1;//dir =1 - направление вверх, speed =0.1 - скорость движения. Заметьте - время мы уже здесь ни на что не умножаем и нигде не используем каждый раз
            CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
            p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 84, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && p.getX() < window.getSize().x) {
            p.dir = 0; p.speed = 0.1;//направление вправо, см выше
            CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
            p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 42, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && p.getY() > 0) {
            p.dir = 3; p.speed = 0.1;//направление вниз, см выше
            CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
            p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 0, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && p.getY() < window.getSize().y) {
            p.dir = 2; p.speed = 0.1;//направление вверх, см выше
            CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
            p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 125, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::P))) {
            int row = p.getY() / 50;
            int col = p.getX() / 50;
            if (p.ability >= 50 && model.getMap().field[row][col] == BlockType::GREEN) {
                model.getMap().life_of_flowers[row][col] = 20;
                model.getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') - 48);
                p.ability = 0;
            }
            else {
                p.ind = 1;
                //std::cout << "До следующей возможности посадить цветок осталось " << 50 - p.ability << " секунд" << "\n";
            }
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
          if (p.bul_ability >= 50) {
              bullets.emplace_back(std::make_unique<Bullet>("bullet.png",p.x,p.y,16.0, 16.0, p.dir));
              p.bul_ability = 0;
          }
          //std::cout << bullets.size() << "\n";
        }
        window.clear();
        switch (model.state)
        {
            case GameState::MENU:
            {
                view.drawMenu();
                break;
            }
            case GameState::GAME:
            {
                p.update(time, model);
                for (size_t i = 0; i < bullets.size(); i++) {
                    (*bullets[i]).update(time, model);
                }
                model.update(time);

                for (size_t i = 0; i < bullets.size(); i++) {
                    if ((*bullets[i]).life == false) {
                        bullets.pop_front();
                    }
                }
                p.ability += 0.001 * time;
                p.bul_ability += 0.001 * time;
                view.drawMap();
                std::wostringstream PlayerScore;
                PlayerScore << p.playerscore;
                text_s.setString(L"Собрано монет:" + PlayerScore.str());
                text_s.setPosition(0, 0);
                window.draw(text_s);

                std::wostringstream PlayerHealth;
                PlayerHealth << p.health;
                text_h.setString(L"Здоровье:" + PlayerHealth.str());
                text_h.setPosition(0, 700);
                window.draw(text_h);
                window.draw(p.sprite);
                for (size_t i = 0; i < bullets.size(); i++) {
                    window.draw((*bullets[i]).sprite);
                }


                if (p.ind == 1) {
                    std::wostringstream M;
                    M << (50 - p.ability);
                    text_m.setString(L"Осталось " + M.str() + L" секунд");
                    text_m.setPosition(450, 0);
                    window.draw(text_m);
                    p.ind = 0;
                }
                if(model.getMap().at((p.getY() + p.h / 2) / BlockSize, (p.getX() + p.w / 2) / BlockSize) == BlockType::WATER){
                    text_m.setString(std::to_string(p.leftOnWater));
                    text_m.setPosition(450, 0);
                    window.draw(text_m);
                }
                break;
            }
            case GameState::DIED:
            {
                view.drawMenu();
                text_m.setString(L"Вы умерли");
                text_m.setPosition(250, 300);
                text_m.setFillColor(sf::Color::Red);
                window.draw(text_m);
                break;
            }
        }
        //std::cout << model.getMap().time_of_life[13][1] << "\n";
        window.display();
    }

    return 0;
}