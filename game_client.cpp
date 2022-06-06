#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include "model.h"
#include "vars.h"
#include "game_server.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <deque>
#include <map>
#include <utility>
#include <thread>
#include <mutex>
#include <cassert>

namespace{
sf::TcpSocket socket;
std::string name;
std::string message = "";
sf::Packet  packet1;
sf::Packet  packet2;
sf::IpAddress ip;
std::mutex m;

sf::RenderWindow window(sf::VideoMode(750, 750), "Game");
std::unique_ptr<Model> model(new ClientModel("../map.txt"));
View view(window, *model);

void F() {
    while (true) {
        if(socket.receive(packet1) == sf::Socket::Done) {
            std::string nameRec;
            std::string s;
            int command;
            packet1 >> nameRec >> command;
            if (model->getPlayers().find(nameRec) == model->getPlayers().end() && command < 6) {
                model->getPlayers().emplace(nameRec, model->getPlayer());
                model->getNames().emplace(nameRec, true);
            }
            if (model->getNames()[nameRec] == true || nameRec == "name") {
                auto& players = model->getPlayers();
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
                        model->getMap().life_of_flowers[row][col] = 20;
                        model->getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') - 48);
                    }
                } else if (command == 4) {
                    static_cast<ClientModel&>(*model).getBullets().emplace_back(
                            std::make_unique<ClientBullet>("bullet.png", players.at(nameRec).x, players.at(nameRec).y, 16.0,
                                                     16.0, players.at(nameRec).dir));
                } else if (command == 5) {
                    packet1 >> players.at(nameRec).x >> players.at(nameRec).y
                            >> players.at(nameRec).health >> players.at(nameRec).playerscore
                            >> players.at(nameRec).ability >> players.at(nameRec).bul_ability
                            >> players.at(nameRec).bullets >> players.at(nameRec).life;
                    model->getNames()[nameRec] = players.at(nameRec).life;
                    int x = players.at(nameRec).x;
                    int y = players.at(nameRec).y;
                    players.at(nameRec).sprite.setPosition(x, y);
                    int rows = model->getMap().getRows();
                    int cols = model->getMap().getCols();
                    for (int i = 0; i < rows; i++) {
                        for (int j = 0; j < cols; j++) {
                            packet1 >> model->getMap().score_of_coins[i][j];
                        }
                    }
                } else if (command == 6) {
                    for (int i = 0; i < model->getMap().getRows(); i++) {
                        for (int j = 0; j < model->getMap().getCols(); j++) {
                            int val;
                            packet1 >> val;
                            if (val == 0) {
                                model->getMap().field[i][j] = BlockType::CONCRETE;
                            } else if (val == 1) {
                                model->getMap().field[i][j] = BlockType::GREEN;
                            } else if (val == 2) {
                                model->getMap().field[i][j] = BlockType::WATER;
                            } else if (val == 3) {
                                model->getMap().field[i][j] = BlockType::MOLE;
                            } else if (val == 4) {
                                model->getMap().field[i][j] = BlockType::SMALL_FLOWER;
                            } else if (val == 5) {
                                model->getMap().field[i][j] = BlockType::AVERAGE_FLOWER;
                            } else if (val == 6) {
                                model->getMap().field[i][j] = BlockType::BIG_FLOWER;
                            }
                        }
                    }
                    for (int i = 0; i < model->getMap().getRows(); i++) {
                        for (int j = 0; j < model->getMap().getCols(); j++) {
                            packet1 >> model->getMap().score_of_coins[i][j];
                        }
                    }
                } else if (command == 7) {
                    int ind;
                    packet1 >> ind;
                    auto& bullet = *(static_cast<ClientModel&>(*model).getBullets())[ind];
                    packet1 >> bullet.x >> bullet.y >> bullet.life;
                    bullet.sprite.setPosition(bullet.x, bullet.y);
                } else if (command == 8) {
                    int bulletInd;
                    packet1 >> bulletInd;
                    static_cast<ClientModel&>(*model).getBullets().erase(static_cast<ClientModel&>(*model).getBullets().begin() + bulletInd);
                } else if (command == 9){
                    static_cast<ClientModel&>(*model).getEnemies().clear();
                    int enemiesCount;
                    packet1 >> enemiesCount;
                    for(int j = 0; j < enemiesCount; j++){
                        int enemyId;
                        packet1 >> enemyId;
                        ClientEnemy& enemy = (static_cast<ClientModel&>(*model).getEnemies())[enemyId];
                        enemy.id = enemyId;
                        packet1 >> enemy.pos.first >> enemy.pos.second >> enemy.hp >> enemy.currFrame;
                        int enemyDir, enemyAction;
                        packet1 >> enemyDir >> enemyAction;
                        enemy.dir = static_cast<Direction>(enemyDir);
                        enemy.action = static_cast<Action>(enemyAction);
                        if(enemy.action == Action::ATTACK){
                            packet1 >> model->getMap().time_hurt[(enemy.pos.first + IMG_H / 2) / BlockSize][(enemy.pos.second + IMG_W / 2) / BlockSize];
                        }
                    }
                }
            }
        }
    }
}
}
void multiGame(){
    //std::cout << "Enter ip: ";
    //std::string Ip;
    //std::cin >> Ip;
    //ip = sf::IpAddress(Ip);
    std::cout << "multiGame called\n";
    ip = sf::IpAddress::getLocalAddress();
    std::cout << "ip " << ip.toString() << "\n";
    if(socket.connect(ip, 2000) != sf::Socket::Done) {
        std::thread t(startServer, 2);
        t.detach();
        if(socket.connect(ip, 2000) != sf::Socket::Done){
            std::cout << "Error\n";
            return;
        }
        std::cout << "NEW SERVR\n";
    }else{
        std::cout << "ALREADY EXISTS\n";
    }
    //while (true) {
    //    std::cout << "Enter your name:\n";
    //    std::cin >> name;
    //    if (model->getPlayers().find(name) == model->getPlayers().end()) {
    //        break;
    //    }
    //    else {
    //        std::cout << "This name is already taken\n";
    //    }
    //}
    std::cout << "Enter your name:\n";
    std::cin >> name;
    socket.setBlocking(false);
    sf::Thread thread(&F);
    thread.launch();


    model->getPlayers().emplace(name, model->getPlayer());
    model->getNames().emplace(name, true);
    model->getNames().emplace("name", true);
    std::cout << "CLIENTMODEL " << model->getPlayers().size() << ' ' << model->getNames().size() << ' ' << model->getMap().getCols() << ' ' << model->getMap().getRows() << "\n";
    std::cout << "MODEL TYPE " << (model->type == 1 ? "SERVER" : "CLIENT") << "\n";
    assert(model->getMap().getCols() > 0 && model->getMap().getRows() > 0);
    packet2.clear();
    packet2 << name << 1;
    socket.send(packet2);
    std::cout << "FFFF " << static_cast<void*>(model.get()) << "\n";

    while (window.isOpen() && model->state == GameState::GAME) {
        sf::Event ev;

        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }else{
                if (ev.type == sf::Event::KeyPressed) {
                    auto& players = model->getPlayers();
                    auto code = ev.key.code;
                    if (model->getNames()[name] == true) {
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
        view.drawMap(model->getPlayers().at(name));
        //window.draw(players.at(name).sprite);
        //std::cout << players.size() << "\n";
        /*for (auto u : players) {
            std::cout << u.first << "\n";
        }*/
        for (auto u : model->getPlayers()) {
            if (model->getNames()[u.first] == true) {
                sf::Text text_p("", *(View::get_or_create_font("font")), 20);
                sf::Text text_ph("", *(View::get_or_create_font("font")), 20);
                //std::wstring widestr = std::wstring(u.first.begin(), u.first.end());
                text_p.setString(u.first);
                text_p.setPosition(model->getPlayers().at(u.first).x, model->getPlayers().at(u.first).y - 20);
                text_ph.setString(u.first);
                text_ph.setPosition(model->getPlayers().at(u.first).x, model->getPlayers().at(u.first).y - 20);
                if (u.first == name) {
                    window.draw(text_ph);
                } else {
                    window.draw(text_p);
                }
                window.draw(model->getPlayers().at(u.first).sprite);
                //std::cout << players.at(u.first).x << " " << players.at(u.first).y << "\n";
            }
        }

        //std::cout << players.at(name).bul_ability << "\n";


        if (model->getPlayers().at(name).ind == 1) {
            std::wostringstream M;
            M << (10 - model->getPlayers().at(name).ability);
            sf::Text text_m("", *(View::get_or_create_font("font")), 20);
            text_m.setString(L"Осталось " + M.str() + L" секунд");
            text_m.setPosition(450, 0);
            window.draw(text_m);
            model->getPlayers().at(name).ind = 0;
        }
        if (model->getNames()[name] == false) {
            model->state = GameState::DIED;
        }
        else if (model->getPlayers().at(name).health <= 0) {
            packet2.clear();
            packet2 << name << 3;
            socket.send(packet2);
        }
        window.display();
    }
}
int main(){
    while(window.isOpen() && (model->state == GameState::MENU || model->state == GameState::DIED)){
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }else if(ev.type == sf::Event::MouseButtonPressed){
                std::cout << "MOUSE CLICKED\n";
                [[maybe_unused]]auto mousePos = sf::Mouse::getPosition(window);
                std::cout << "clicked\n";
                std::cout << "Before model new\n";
                model = std::move(std::unique_ptr<ClientModel>(new ClientModel("../map.txt")));
                std::cout << "after model nem\n";
                std::cout << model->getMap().getCols() << ' ' << model->getMap().getRows() << '\n';
                model->state = GameState::GAME;
                view.changeModel(*model);
                multiGame();
                //if(model->getButtons()[0].isClicked(mousePos.x, mousePos.y)){
                //    model = std::move(std::unique_ptr<ServerModel>(new ServerModel("map.txt", 1 + rand() % 6)));
                //    model->state = GameState::GAME;
                //    view.changeModel(*model);
                //    multiGame();
                //}else if(model->getButtons()[1].isClicked(mousePos.x, mousePos.y)){
                //    window.close();
                //}else if(model->getButtons()[2].isClicked(mousePos.x, mousePos.y)){
                //    model = std::move(std::unique_ptr<ClientModel>(new ClientModel("map.txt")));
                //    model->state = GameState::GAME;
                //    view.changeModel(*model);
                //    multiGame();
                //}

            }
        }
        window.clear(sf::Color(100, 100, 100));
        view.drawMenu();
        window.display();
    }
}

/*
g++ -std=c++17 game_client.cpp model.cpp map.cpp player.cpp view.cpp enemy.cpp -o game_client -lsfml-window -lsfml-system -lsfml-graphics -lsfml-network
g++ -std=c++17 game_server.cpp model.cpp map.cpp player.cpp view.cpp enemy.cpp -o game_server -lsfml-window -lsfml-system -lsfml-graphics -lsfml-network
*/