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
std::map <std::string, bool> names;
Model model("map.txt");

void F() {
    while (true) {
        if(socket.receive(packet1) == sf::Socket::Done) {
            std::string nameRec;
            std::string s;
            int command;
            packet1 >> nameRec >> command;
            if (players.find(nameRec) == players.end() && command < 6) {
                players.emplace(nameRec, model.getPlayer());
                names.emplace(nameRec, true);
            }
            if (names[nameRec] == true || nameRec == "name") {
                if (command == 2) {
                    packet1 >> players.at(nameRec).dir >> players.at(nameRec).speed >> players.at(nameRec).CurrentFrame
                            >> s;
                    if (s == "L") {
                        players.at(nameRec).sprite.setTextureRect(
                                sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 84, 25, 42));
                    } else if (s == "R") {
                        players.at(nameRec).sprite.setTextureRect(
                                sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 42, 25, 42));
                    } else if (s == "U") {
                        players.at(nameRec).sprite.setTextureRect(
                                sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 0, 25, 42));
                    } else if (s == "D") {
                        players.at(nameRec).sprite.setTextureRect(
                                sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 125, 25, 42));
                    }
                } else if (command == 3) {
                    int row, col;
                    packet1 >> row >> col >> players.at(nameRec).ind;
                    if (players.at(nameRec).ind == 0) {
                        model.getMap().life_of_flowers[row][col] = 20;
                        model.getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') - 48);
                    }
                } else if (command == 4) {
                    bullets.emplace_back(
                            std::make_unique<Bullet>("bullet.png", players.at(nameRec).x, players.at(nameRec).y, 16.0,
                                                     16.0, players.at(nameRec).dir));
                } else if (command == 5) {
                    packet1 >> players.at(nameRec).x >> players.at(nameRec).y >> players.at(nameRec).leftOnWater
                            >> players.at(nameRec).health >> players.at(nameRec).playerscore
                            >> players.at(nameRec).ability >> players.at(nameRec).bul_ability
                            >> players.at(nameRec).bullets >> players.at(nameRec).life;
                    names[nameRec] = players.at(nameRec).life;
                    int x = players.at(nameRec).x;
                    int y = players.at(nameRec).y;
                    players.at(nameRec).sprite.setPosition(x, y);
                    int rows = model.getMap().getRows();
                    int cols = model.getMap().getCols();
                    for (int i = 0; i < rows; i++) {
                        for (int j = 0; j < cols; j++) {
                            packet1 >> model.getMap().score_of_coins[i][j];
                        }
                    }
                } else if (command == 6) {
                    for (int i = 0; i < model.getMap().getRows(); i++) {
                        for (int j = 0; j < model.getMap().getCols(); j++) {
                            int val;
                            packet1 >> val;
                            if (val == 0) {
                                model.getMap().field[i][j] = BlockType::CONCRETE;
                            } else if (val == 1) {
                                model.getMap().field[i][j] = BlockType::GREEN;
                            } else if (val == 2) {
                                model.getMap().field[i][j] = BlockType::WATER;
                            } else if (val == 3) {
                                model.getMap().field[i][j] = BlockType::MOLE;
                            } else if (val == 4) {
                                model.getMap().field[i][j] = BlockType::SMALL_FLOWER;
                            } else if (val == 5) {
                                model.getMap().field[i][j] = BlockType::AVERAGE_FLOWER;
                            } else if (val == 6) {
                                model.getMap().field[i][j] = BlockType::BIG_FLOWER;
                            }
                        }
                    }
                    for (int i = 0; i < model.getMap().getRows(); i++) {
                        for (int j = 0; j < model.getMap().getCols(); j++) {
                            packet1 >> model.getMap().score_of_coins[i][j];
                        }
                    }
                } else if (command == 7) {
                    int ind;
                    packet1 >> ind;
                    packet1 >> (*bullets[ind]).x >> (*bullets[ind]).y >> (*bullets[ind]).life;
                    (*bullets[ind]).sprite.setPosition((*bullets[ind]).x, (*bullets[ind]).y);
                } else if (command == 8) {
                    bullets.pop_front();
                }
            }
        }
    }
}

int main() {
    //ip = sf::IpAddress::getLocalAddress();
    //std::cout << ip.toString() << "\n";
    //ip = sf::IpAddress("192.168.43.253");
    std::cout << "Enter ip: ";
    std::string Ip;
    std::cin >> Ip;
    ip = sf::IpAddress(Ip);
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
    names.emplace(name, true);
    names.emplace("name", true);
    packet2.clear();
    packet2 << name << 1;
    socket.send(packet2);

    sf::Clock clock;

    sf::Font score;
    score.loadFromFile("arial.ttf");
    sf::Text text_s("", score, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_s.setFillColor(sf::Color::Yellow);
    text_s.setStyle(sf::Text::Bold);

    sf::Font Health;
    Health.loadFromFile("arial.ttf");
    sf::Text text_h("", Health, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_h.setFillColor(sf::Color::Green);
    text_h.setStyle(sf::Text::Bold);

    sf::Font Message;
    Message.loadFromFile("arial.ttf");
    sf::Text text_m("", Message, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_m.setFillColor(sf::Color::Red);
    text_m.setStyle(sf::Text::Bold);

    sf::Font player_en;
    player_en.loadFromFile("arial.ttf");
    sf::Text text_p("", player_en, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_p.setFillColor(sf::Color::Black);
    text_p.setStyle(sf::Text::Bold);

    sf::Font player_h;
    player_h.loadFromFile("arial.ttf");
    sf::Text text_ph("", player_h, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_ph.setFillColor(sf::Color::Red);
    text_ph.setStyle(sf::Text::Bold);

    sf::Font bul;
    bul.loadFromFile("arial.ttf");
    sf::Text text_bul("", bul, 20);
    //text.setOutlineColor(sf::Color::Black);
    text_bul.setFillColor(sf::Color::Black);
    text_bul.setStyle(sf::Text::Bold);

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
                    if (names[name] == true) {
                        if ((code == sf::Keyboard::Left) && players.at(name).x >= 0) {
                            packet2.clear();
                            packet2 << name << 2 << "L";
                            socket.send(packet2);
                        } else if ((code == sf::Keyboard::Right) && players.at(name).x < (window.getSize().x - 1)) {
                            packet2.clear();
                            packet2 << name << 2 << "R";
                            socket.send(packet2);
                        } else if ((code == sf::Keyboard::Up) && players.at(name).y >= 0) {
                            packet2.clear();
                            packet2 << name << 2 << "U";
                            socket.send(packet2);
                        } else if ((code == sf::Keyboard::Down) && players.at(name).y < (window.getSize().y - 1)) {
                            packet2.clear();
                            packet2 << name << 2 << "D";
                            socket.send(packet2);
                        } else if (code == sf::Keyboard::P) {
                            int row = players.at(name).y / 50;
                            int col = players.at(name).x / 50;
                            packet2.clear();
                            packet2 << name << 2 << "P";
                            socket.send(packet2);
                            players.at(name).ability = 0;
                        } else if (code == sf::Keyboard::S) {
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

                std::wostringstream PlayerBullets;
                PlayerBullets << players.at(name).bullets;
                text_bul.setString(L"Количество патронов:" + PlayerBullets.str());
                text_bul.setPosition(450, 700);
                window.draw(text_bul);
                //window.draw(players.at(name).sprite);
                //std::cout << players.size() << "\n";
                /*for (auto u : players) {
                    std::cout << u.first << "\n";
                }*/
                for (auto u : players) {
                    if (names[u.first] == true) {
                        //std::wstring widestr = std::wstring(u.first.begin(), u.first.end());
                        text_p.setString(u.first);
                        text_p.setPosition(players.at(u.first).x, players.at(u.first).y - 20);
                        text_ph.setString(u.first);
                        text_ph.setPosition(players.at(u.first).x, players.at(u.first).y - 20);
                        if (u.first == name) {
                            window.draw(text_ph);
                        } else {
                            window.draw(text_p);
                        }
                        window.draw(players.at(u.first).sprite);
                        //std::cout << players.at(u.first).x << " " << players.at(u.first).y << "\n";
                    }
                }
                for (size_t i = 0; i < bullets.size(); i++) {
                    window.draw((*bullets[i]).sprite);
                }

                //std::cout << players.at(name).bul_ability << "\n";


                if (players.at(name).ind == 1) {
                    std::wostringstream M;
                    M << (10 - players.at(name).ability);
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
        if (names[name] == false) {
            model.state = GameState::DIED;
        }
        else if (players.at(name).leftOnWater <= 0) {
            packet2.clear();
            packet2 << name << 3;
            socket.send(packet2);
        }
        //std::cout << names["b"] << "\n";
        window.display();
    }
}