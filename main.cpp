#include "model.h"
#include "vars.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <deque>
#include <iostream>
#include <sstream>
#include <utility>
class Bullet {
  private:
    float x, y;

  public:
    float w, h, dx, dy, speed;
    bool life;
    int dir;
    int force = 10;
    sf::String File;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    Bullet(sf::String F, float X, float Y, float W, float H, int dir_) {
        dx = 0;
        dy = 0;
        speed = BlockSize;
        dir = dir_;
        life = true;
        File = F;
        w = W;
        h = H;
        image.loadFromFile("../img/" + File);
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        x = X;
        y = Y; //координата появления спрайта
        sprite.setTextureRect(sf::IntRect(0, 0, w, h));
        sprite.setPosition(x, y);
    }

    void update(float time, Model &model) {
        switch (dir) {
        case 0:
            dx = speed;
            dy = 0;
            break;
        case 1:
            dx = -speed;
            dy = 0;
            break;
        case 2:
            dx = 0;
            dy = speed;
            break;
        case 3:
            dx = 0;
            dy = -speed;
            break;
        }

        x += dx * time;
        y += dy * time;

        sprite.setPosition(x, y);
        interactionWithMap(model);
    }

    void interactionWithMap(Model &model) {
        for (int i = y / 50; i < (y + h) / 50; i++) {
            for (int j = x / 50; j < (x + w) / 50; j++) {
                if (i <= 0 || j <= 0 || i >= (model.getMap().getRows() - 1) ||
                    j >= (model.getMap().getCols() - 1)) {
                    life = false;
                } else if (model.getMap().field[i][j] ==
                               BlockType::SMALL_FLOWER ||
                           model.getMap().field[i][j] ==
                               BlockType::AVERAGE_FLOWER ||
                           model.getMap().field[i][j] ==
                               BlockType::BIG_FLOWER) {
                    life = false;
                    model.getMap().life_of_flowers[i][j] -= 20;
                }
            }
        }
    }

    float getX() { return x; }

    float getY() { return y; }
};

std::deque<std::unique_ptr<Bullet>> bullets;

int main() {
    sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
    std::unique_ptr<Model> model_ptr(new Model("../map.txt"));
    View view(window, *model_ptr);

    Player *p =
        &(model_ptr->getPlayer()); //создаем объект p класса player,задаем
                                   //"hero.png" как имя файла+расширение, далее
                                   //координата Х,У, ширина, высота.

    float CurrentFrame = 0; //хранит текущий кадр
    sf::Clock clock;

    sf::Font score;
    score.loadFromFile("../arial.ttf");
    sf::Text text_s("", score, 20);
    // text.setOutlineColor(sf::Color::Black);
    text_s.setFillColor(sf::Color::Black);
    text_s.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Font Health;
    Health.loadFromFile("../arial.ttf");
    sf::Text text_h("", Health, 20);
    // text.setOutlineColor(sf::Color::Black);
    text_h.setFillColor(sf::Color::Black);
    text_h.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Font Message;
    Message.loadFromFile("../arial.ttf");
    sf::Text text_m("", Message, 20);
    // text.setOutlineColor(sf::Color::Black);
    text_m.setFillColor(sf::Color::Red);
    text_m.setStyle(sf::Text::Bold | sf::Text::Underlined);

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        sf::Event ev;

        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }
            if (model_ptr->state == GameState::MENU ||
                model_ptr->state == GameState::DIED) {
                if (ev.type == sf::Event::MouseButtonPressed) {
                    auto mousePos = sf::Mouse::getPosition(window);
                    if (model_ptr->getButtons()[0].isClicked(
                            mousePos.x, mousePos.y)) { // нажали "Single game"
                        model_ptr = std::make_unique<Model>("../map.txt");
                        model_ptr->state = GameState::GAME;
                        view.changeModel(*model_ptr);
                        p = &(model_ptr->getPlayer());
                    } else if (model_ptr->getButtons()[1].isClicked(
                                   mousePos.x, mousePos.y)) { // нажали "Exit"
                        window.close();
                    }
                }
                break;
            }
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) &&
            p->x > 0) { //если нажата клавиша стрелка влево или англ буква А
            p->dir = 1;
            p->speed = PlayerSpeed; // dir =1 - направление вверх, speed =0.1 -
                                    // скорость движения. Заметьте - время мы уже
                                    // здесь ни на что не умножаем и нигде не
                                    // используем каждый раз
            CurrentFrame +=
                5 *
                time; //служит для прохождения по "кадрам". переменная доходит
                      //до трех суммируя произведение времени и скорости.
                      //изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4)
                CurrentFrame -=
                    4; //проходимся по кадрам с первого по третий включительно.
                       //если пришли к третьему кадру - откидываемся назад.
            p->sprite.setTextureRect(
                sf::IntRect(25 * int(CurrentFrame), 84, 25,
                            42)); //проходимся по координатам Х. получается
                                  //96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) &&
            p->x < window.getSize().x) {
            p->dir = 0;
            p->speed = PlayerSpeed; //направление вправо, см выше
            CurrentFrame +=
                5 *
                time; //служит для прохождения по "кадрам". переменная доходит
                      //до трех суммируя произведение времени и скорости.
                      //изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4)
                CurrentFrame -=
                    4; //проходимся по кадрам с первого по третий включительно.
                       //если пришли к третьему кадру - откидываемся назад.
            p->sprite.setTextureRect(
                sf::IntRect(25 * int(CurrentFrame), 42, 25,
                            42)); //проходимся по координатам Х. получается
                                  //96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && p->y > 0) {
            p->dir = 3;
            p->speed = PlayerSpeed; //направление вниз, см выше
            CurrentFrame +=
                5 *
                time; //служит для прохождения по "кадрам". переменная доходит
                      //до трех суммируя произведение времени и скорости.
                      //изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4)
                CurrentFrame -=
                    4; //проходимся по кадрам с первого по третий включительно.
                       //если пришли к третьему кадру - откидываемся назад.
            p->sprite.setTextureRect(
                sf::IntRect(25 * int(CurrentFrame), 0, 25,
                            42)); //проходимся по координатам Х. получается
                                  //96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) &&
            p->y < window.getSize().y) {
            p->dir = 2;
            p->speed = PlayerSpeed; //направление вверх, см выше
            CurrentFrame +=
                5 *
                time; //служит для прохождения по "кадрам". переменная доходит
                      //до трех суммируя произведение времени и скорости.
                      //изменив 0.005 можно изменить скорость анимации
            if (CurrentFrame > 4)
                CurrentFrame -=
                    4; //проходимся по кадрам с первого по третий включительно.
                       //если пришли к третьему кадру - откидываемся назад.
            p->sprite.setTextureRect(
                sf::IntRect(25 * int(CurrentFrame), 125, 25,
                            42)); //проходимся по координатам Х. получается
                                  //96,96*2,96*3 и опять 96
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::P))) {
            int row = p->y / 50;
            int col = p->x / 50;
            if (p->ability >= 50 &&
                model_ptr->getMap().field[row][col] == BlockType::GREEN) {
                model_ptr->getMap().life_of_flowers[row][col] = 20;
                model_ptr->getMap().field[row][col] =
                    static_cast<BlockType>(static_cast<int>('4') - 48);
                p->ability = 0;
            } else {
                p->ind = 1;
                // std::cout << "До следующей возможности посадить цветок
                // осталось " << 50 - p.ability << " секунд" << "\n";
            }
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
            if (p->bul_ability >= 50) {
                bullets.emplace_back(std::make_unique<Bullet>(
                    "bullet.png", p->x, p->y, 16.0, 16.0, p->dir));
                p->bul_ability = 0;
            }
            // std::cout << bullets.size() << "\n";
        }
        window.clear();
        switch (model_ptr->state) {
        case GameState::MENU: {
            view.drawMenu();
            break;
        }
        case GameState::GAME: {
            p->update(time, *model_ptr);
            for (size_t i = 0; i < bullets.size(); i++) {
                (*bullets[i]).update(time, *model_ptr);
                for (std::unique_ptr<Enemy> &enemy_ptr :
                     model_ptr->getEnemies()) {
                    bool ok_x =
                        (enemy_ptr->getPos().second <= bullets[i]->getX() &&
                         bullets[i]->getX() <=
                             enemy_ptr->getPos().second + enemy_ptr->w);
                    bool ok_y =
                        (enemy_ptr->getPos().first <= bullets[i]->getY() &&
                         bullets[i]->getY() <=
                             enemy_ptr->getPos().first + enemy_ptr->h);
                    if (ok_x && ok_y) {
                        bullets[i]->life = false;
                        enemy_ptr->hp -= bullets[i]->force;
                        break;
                    }
                }
            }
            model_ptr->update(time);
            for (size_t i = 0; i < bullets.size(); i++) {
                if ((*bullets[i]).life == false) {
                    bullets.pop_front();
                }
            }
            p->ability += time;
            p->bul_ability += time;
            view.drawMap();
            std::wostringstream PlayerScore;
            PlayerScore << p->playerscore;
            text_s.setString(L"Собрано монет:" + PlayerScore.str());
            text_s.setPosition(0, 0);
            window.draw(text_s);

            std::wostringstream PlayerHealth;
            PlayerHealth << p->health;
            text_h.setString(L"Здоровье:" + PlayerHealth.str());
            text_h.setPosition(0, 700);
            window.draw(text_h);
            window.draw(p->sprite);
            for (size_t i = 0; i < bullets.size(); i++) {
                window.draw((*bullets[i]).sprite);
            }

            if (p->ind == 1) {
                std::wostringstream M;
                M << (50 - p->ability);
                text_m.setString(L"Осталось " + M.str() + L" секунд");
                text_m.setPosition(450, 0);
                window.draw(text_m);
                p->ind = 0;
            }
            for (std::unique_ptr<Enemy> &enemy : model_ptr->getEnemies()) {
                view.drawEnemy(*enemy, time);
            }
            break;
        }
        case GameState::DIED: {
            view.drawMenu();
            text_m.setString(L"Вы умерли");
            text_m.setPosition(250, 300);
            text_m.setFillColor(sf::Color::Red);
            window.draw(text_m);
            break;
        }
        }
        // std::cout << model.getMap().time_of_life[13][1] << "\n";
        window.display();
    }

    return 0;
}