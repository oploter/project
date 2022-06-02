#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "model.h"
#include "vars.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <deque>
#include <utility>
#include <mutex>

sf::TcpSocket socket;
std::string name;
std::string message = "";
sf::Packet  packet1;
sf::Packet  packet2;
sf::IpAddress ip;
std::mutex m;

struct Bullet {
    float x, y;
    float w, h, dx, dy, speed;
    bool life;
    int dir;
    sf::String File;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    Bullet(sf::String F, float X, float Y, float W, float H, int dir_){
        dx=0;dy=0;speed=0.15;dir=dir_;
        life = true;
        File = F;
        w = W; h = H;
        image.loadFromFile("img/" + File);
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        x = X; y = Y;//координата появления спрайта
        sprite.setTextureRect(sf::IntRect(0, 0, w, h));
        sprite.setPosition(x, y);
    }
};

std::deque <std::unique_ptr<Bullet>> bullets;
std::map <std::string, Player> players;
Model model("map.txt");

void F() {
    while (true) {
        if(socket.receive(packet1) == sf::Socket::Done) {
            std::string nameRec;
            std::string s;
            int command;
            packet1 >> nameRec >> command;
            if (players.find(nameRec) == players.end() && command != 6 && command != 7) {
                players.emplace(nameRec, model.getPlayer());
            }

            if (command == 2) {
                packet1 >> players.at(nameRec).dir >> players.at(nameRec).speed >> players.at(nameRec).CurrentFrame >> s;
                if (s == "L") {
                    players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 84, 25, 42));
                }
                else if (s == "R") {
                    players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 42, 25, 42));
                }
                else if (s == "U") {
                    players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 0, 25, 42));
                }
                else if (s == "D") {
                    players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 125, 25, 42));
                }
            }
            else if (command == 3) {
                int row, col;
                packet1 >> row >> col;
                model.getMap().life_of_flowers[row][col] = 20;
                model.getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') - 48);
            }
            else if (command == 4) {
                bullets.emplace_back(std::make_unique<Bullet>("bullet.png",players.at(nameRec).x,players.at(nameRec).y,16.0, 16.0, players.at(nameRec).dir));
            }
            else if (command == 5) {
                packet1 >> players.at(nameRec).x >> players.at(nameRec).y >> players.at(nameRec).leftOnWater >> players.at(nameRec).health >> players.at(nameRec).playerscore;
                int x = players.at(nameRec).x;
                int y = players.at(nameRec).y;
                players.at(nameRec).sprite.setPosition(x,y);
                int rows = model.getMap().getRows();
                int cols = model.getMap().getCols();
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        packet1 >> model.getMap().score_of_coins[i][j];
                    }
                }
            }
            else if (command == 6) {
                for (int i = 0; i < model.getMap().getRows(); i++) {
                    for (int j = 0; j < model.getMap().getCols(); j++) {
                        int val;
                        packet1 >> val;
                        if (val == 0) {
                            model.getMap().field[i][j] = BlockType::CONCRETE;
                        }
                        else if (val == 1) {
                            model.getMap().field[i][j] = BlockType::GREEN;
                        }
                        else if (val == 2) {
                            model.getMap().field[i][j] = BlockType::WATER;
                        }
                        else if (val == 3) {
                            model.getMap().field[i][j] = BlockType::MOLE;
                        }
                        else if (val == 4) {
                            model.getMap().field[i][j] = BlockType::SMALL_FLOWER;
                        }
                        else if (val == 5) {
                            model.getMap().field[i][j] = BlockType::AVERAGE_FLOWER;
                        }
                        else if (val == 6) {
                            model.getMap().field[i][j] = BlockType::BIG_FLOWER;
                        }
                    }
                }
                for (int i = 0; i < model.getMap().getRows(); i++) {
                    for (int j = 0; j < model.getMap().getCols(); j++) {
                        packet1 >> model.getMap().score_of_coins[i][j];
                    }
                }
            }
            else if (command == 7) {
                int ind;
                packet1 >> ind;
                packet1 >> (*bullets[ind]).x >> (*bullets[ind]).y >> (*bullets[ind]).life;
                (*bullets[ind]).sprite.setPosition((*bullets[ind]).x,(*bullets[ind]).y);
                std::cout << (*bullets[ind]).x << " " << (*bullets[ind]).y << " " << (*bullets[ind]).life << " " << bullets.size() << "\n";
            }
        }
    }
}

int main() {
    ip = sf::IpAddress::getLocalAddress();
    if(socket.connect(ip, 2000) != sf::Socket::Done) {
        std::cout << "Error!\n";
    }
    while (true) {
        std::cout << "Enter your name:\n";
        std::cin >> name;
        if (players.find(name) == players.end()) {
            break;
        }
        else {
            std::cout << "This name is already taken\n";
        }
    }
    socket.setBlocking(false);
    sf::Thread thread(&F);
    thread.launch();

    sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
    View view(window, model);

    players.emplace(name, model.getPlayer());
    packet2.clear();
    packet2 << name << 1;
    socket.send(packet2);

    sf::Clock clock;

    sf::Font score;
    score.loadFromFile("arial.ttf");
    sf::Text text_s("", score, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_s.setFillColor(sf::Color::Black);
    text_s.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Font Health;
    Health.loadFromFile("arial.ttf");
    sf::Text text_h("", Health, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_h.setFillColor(sf::Color::Black);
    text_h.setStyle(sf::Text::Bold | sf::Text::Underlined);

    sf::Font Message;
    Message.loadFromFile("arial.ttf");
    sf::Text text_m("", Message, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_m.setFillColor(sf::Color::Red);
    text_m.setStyle(sf::Text::Bold | sf::Text::Underlined);

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time = time / 10;
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
            else {
                if (ev.type == sf::Event::KeyPressed) {
                    auto code = ev.key.code;
                    if ((code == sf::Keyboard::Left) && players.at(name).x >= 0) {
                        packet2.clear();
                        packet2 << name << 2 << "L";
                        socket.send(packet2);
                    }

                    else if ((code == sf::Keyboard::Right) && players.at(name).x < (window.getSize().x - 1)) {
                        packet2.clear();
                        packet2 << name << 2 << "R";
                        socket.send(packet2);
                    }

                    else if ((code == sf::Keyboard::Up) && players.at(name).y >= 0) {
                        packet2.clear();
                        packet2 << name << 2 << "U";
                        socket.send(packet2);
                    }

                    else if ((code == sf::Keyboard::Down) && players.at(name).y < (window.getSize().y - 1)) {
                        packet2.clear();
                        packet2 << name << 2 << "D";
                        socket.send(packet2);
                    }

                    else if (code == sf::Keyboard::P) {
                        int row = players.at(name).y / 50;
                        int col = players.at(name).x/ 50;
                        if (players.at(name).ability >= 50 && model.getMap().field[row][col] == BlockType::GREEN) {
                            packet2.clear();
                            packet2 << name << 2 << "P";
                            socket.send(packet2);
                            players.at(name).ability = 0;
                        }
                        else {
                            players.at(name).ind = 1;
                            //std::cout << "До следующей возможности посадить цветок осталось " << 50 - p.ability << " секунд" << "\n";
                        }
                    }

                    else if (code == sf::Keyboard::S) {
                        if (players.at(name).bul_ability >= 1) {
                            packet2.clear();
                            packet2 << name << 2 << "S";
                            socket.send(packet2);
                            players.at(name).bul_ability = 0;
                        }
                    }
                }
            }
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
                for (size_t i = 0; i < bullets.size(); i++) {
                    if ((*bullets[i]).life == false) {
                        std::cout << "YES" << "\n";
                        //bullets.pop_front();
                    }
                }

                players.at(name).ability += 0.001 * time;
                players.at(name).bul_ability += 0.001 * time;
                view.drawMap();
                std::wostringstream PlayerScore;
                PlayerScore << players.at(name).playerscore;
                text_s.setString(L"Собрано монет:" + PlayerScore.str());
                text_s.setPosition(0, 0);
                window.draw(text_s);

                std::wostringstream PlayerHealth;
                PlayerHealth << players.at(name).health;
                text_h.setString(L"Здоровье:" + PlayerHealth.str());
                text_h.setPosition(0, 700);
                window.draw(text_h);
                //window.draw(players.at(name).sprite);
                /*std::cout << players.size() << "\n";
                for (auto u : players) {
                    std::cout << u.first << "\n";
                }*/
                for (auto u : players) {
                    window.draw(players.at(u.first).sprite);
                }
                for (size_t i = 0; i < bullets.size(); i++) {
                    window.draw((*bullets[i]).sprite);
                }


                if (players.at(name).ind == 1) {
                    std::wostringstream M;
                    M << (50 - players.at(name).ability);
                    text_m.setString(L"Осталось " + M.str() + L" секунд");
                    text_m.setPosition(450, 0);
                    window.draw(text_m);
                    players.at(name).ind = 0;
                }
                if(model.getMap().at((players.at(name).y + players.at(name).h / 2) / BlockSize, (players.at(name).x + players.at(name).w / 2) / BlockSize) == BlockType::WATER){
                    text_m.setString(std::to_string(players.at(name).leftOnWater));
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
}