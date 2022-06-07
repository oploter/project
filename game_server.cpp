#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "model.h"
#include "vars.h"
#include "bullet.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <utility>
#include <thread>

namespace{
ServerModel model("map.txt", 1 + (rand() % 5));

sf::Clock clock1;
float time1;
int bulletCount = 0;
std::size_t count;
std::string message = "";
sf::Packet packet1, packet2, packet3, packetWithEnemies;
sf::TcpListener listener;
sf::SocketSelector selector;
std::mutex m;
std::vector <std::unique_ptr<sf::TcpSocket*>> clients;


void F() {
    while (true) {
        for (auto it = clients.begin(); it != clients.end(); it++) {
            sf::TcpSocket* client = **it;
            if (selector.isReady(*client)) {
                if ((*client).receive(packet1) == sf::Socket::Done) {
                    m.lock();
                    std::string nameRec;
                    std::string messageRec;
                    int command;
                    //std::cout << packet.getDataSize() << "\n";
                    packet1 >> nameRec;
                    if (model.getPlayers().find(nameRec) == model.getPlayers().end()) {
                        model.getPlayers().emplace(nameRec, model.getPlayer());
                        model.getNames().emplace(nameRec, true);
                        if (model.getPlayers().size() <= count) {
                            for (auto name : model.getPlayers()) {
                                packet2.clear();
                                packet2 << name.first << 1;
                                for (auto t = clients.begin(); t != clients.end(); t++) {
                                    sf::TcpSocket *cl = **t;
                                    if (selector.isReady(*cl)) {
                                        (*cl).send(packet2);
                                    }
                                }
                            }
                        }
                        if (model.getPlayers().size() == count) {
                            count = 0;
                        }
                    }

                    packet1 >> command;

                    if (model.getNames()[nameRec] == true) {
                        if (command == 2) {
                            std::string button;
                            packet1 >> button;
                            auto& players = model.getPlayers();
                            if (button == "L") {
                                players.at(nameRec).dir = 1;
                                players.at(nameRec).speed = BlockSize* 2;
                                players.at(nameRec).CurrentFrame += 0.005 * time1;
                                if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                                players.at(nameRec).sprite.setTextureRect(
                                        sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 84, 25, 42));
                                packet2.clear();
                                packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed
                                        << players.at(nameRec).CurrentFrame << "L";
                            } else if (button == "R") {
                                players.at(nameRec).dir = 0;
                                players.at(nameRec).speed = BlockSize * 2;
                                players.at(nameRec).CurrentFrame += 0.005 * time1;
                                if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                                players.at(nameRec).sprite.setTextureRect(
                                        sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 42, 25, 42));
                                packet2.clear();
                                packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed
                                        << players.at(nameRec).CurrentFrame << "R";
                            } else if (button == "U") {
                                players.at(nameRec).dir = 3;
                                players.at(nameRec).speed = BlockSize * 2;
                                players.at(nameRec).CurrentFrame += 0.005 * time1;
                                if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                                players.at(nameRec).sprite.setTextureRect(
                                        sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 0, 25, 42));
                                packet2.clear();
                                packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed
                                        << players.at(nameRec).CurrentFrame << "U";
                            } else if (button == "D") {
                                players.at(nameRec).dir = 2;
                                players.at(nameRec).speed = BlockSize * 2;
                                players.at(nameRec).CurrentFrame += 0.005 * time1;
                                if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                                players.at(nameRec).sprite.setTextureRect(
                                        sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 125, 25, 42));
                                packet2.clear();
                                packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed
                                        << players.at(nameRec).CurrentFrame << "D";
                            } else if (button == "P") {
                                int row = players.at(nameRec).y / 50;
                                int col = players.at(nameRec).x / 50;
                                if (players.at(nameRec).ability >= 10) {
                                    if (model.getMap().field[row][col] == BlockType::GREEN) {
                                        model.getMap().life_of_flowers[row][col] = 20;
                                        model.getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') -
                                                                                                48);
                                        players.at(nameRec).ability = 0;
                                    }
                                } else {
                                    players.at(nameRec).ind = 1;
                                }
                                packet2.clear();
                                packet2 << nameRec << 3 << row << col << players.at(nameRec).ind;
                                players.at(nameRec).ind = 0;
                            } else if (button == "S") {
                                if (players.at(nameRec).bul_ability >= 10 && players.at(nameRec).bullets > 0) {
                                    model.getBullets().emplace_back("bullet.png", players.at(nameRec).x, players.at(nameRec).y, 16.0, 16.0, players.at(nameRec).dir, nameRec, bulletCount);
                                    bulletCount++;
                                    packet2.clear();
                                    packet2 << nameRec << 4 << (model.getBullets().size() - 1);
                                    std::cout << "new bullet " << model.getBullets().back().id << "\n";
                                    players.at(nameRec).bul_ability = 0;
                                    players.at(nameRec).bullets -= 1;
                                }
                            }
                            for (auto t = clients.begin(); t != clients.end(); t++) {
                                sf::TcpSocket *cl = **t;
                                if (selector.isReady(*cl)) {
                                    (*cl).send(packet2);
                                }
                            }
                        }
                    }
                    if (command == 3) {
                        model.getPlayers().at(nameRec).life = false;
                    }

                    m.unlock();
                }
            }
        }
    }
}
void G() {
    while (true) {
        if (selector.wait()) {
            if (selector.isReady(listener)) {
                m.lock();
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done) {
                    (*client).setBlocking(false);
                    clients.emplace_back(std::make_unique<sf::TcpSocket*>(client));
                    selector.add(*client);
                }
                else {
                    delete client;
                }
                m.unlock();
            }
        }
    }
}
}
int main() {
    listener.listen(2000);
    selector.add(listener);
    sf::Thread thread1(&G);
    thread1.launch();
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    std::cout << "Ip: " << ip.toString() << "\n";
    std::cout << "Enter count of players:\n";
    std::cin >> count;
    sf::Thread thread2(&F);
    thread2.launch();
    std::string mz = "f";

    while(true) {
        time1 = clock1.getElapsedTime().asMicroseconds();
        clock1.restart();
        time1 = time1 / 100;

        /*packet2.clear();
        packet2 << "name" << 0 << time1;
        for (auto t = clients.begin(); t != clients.end(); t++) {
            sf::TcpSocket *cl = **t;
            if (selector.isReady(*cl)) {
                (*cl).send(packet2);
            }
        }*/
        for (auto &u : model.getPlayers()) {
            if (model.getNames()[u.first] == true) {
                u.second.update(time1, model, u.first);
                u.second.ability += 0.001 * time1;
                u.second.bul_ability += 0.001 * time1;
                // std::cout << u.second.x << " " << u.second.y << " " << u.second.speed << " " << u.second.dir << "\n";
                packet3.clear();
                packet3 << u.first << 5 << u.second.x << u.second.y << u.second.health
                        << u.second.playerscore << u.second.ability << u.second.bul_ability << u.second.bullets
                        << u.second.life;
                int rows = model.getMap().getRows();
                int cols = model.getMap().getCols();
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < cols; j++) {
                        packet3 << model.getMap().score_of_coins[i][j];
                    }
                }
                for (auto t = clients.begin(); t != clients.end(); t++) {
                    sf::TcpSocket *cl = **t;
                    if (selector.isReady(*cl)) {
                        (*cl).send(packet3);
                    }
                }
            }
        }
        for (size_t i = 0; i < model.getBullets().size(); i++) {
            auto& bullet = model.getBullets()[i];
            bullet.update(time1, model);
            packet3.clear();
            if(bullet.life == false){
                packet3 << "name" << 8 << bullet.id;
                std::swap(model.getBullets()[i], model.getBullets().back());
                model.getBullets().pop_back();
                i--;
                continue;
            }
            packet3 << "name" << 7 << bullet.id << bullet.x << bullet.y << bullet.life;
            for (auto t = clients.begin(); t != clients.end(); t++) {
                sf::TcpSocket *cl = **t;
                if (selector.isReady(*cl)) {
                    (*cl).send(packet3);
                }
            }
        }
        model.update(time1);
        packet3.clear();
        packet3 << "name" << 6;
        for (int i = 0; i < model.getMap().getRows(); i++) {
            for (int j = 0; j < model.getMap().getCols(); j++) {
                if (model.getMap().field[i][j] == BlockType::CONCRETE) {
                    packet3 << 0;
                }
                else if (model.getMap().field[i][j] == BlockType::GREEN) {
                    packet3 << 1;
                }
                else if (model.getMap().field[i][j] == BlockType::WATER) {
                    packet3 << 2;
                }
                else if (model.getMap().field[i][j] == BlockType::MOLE) {
                    packet3 << 3;
                }
                else if (model.getMap().field[i][j] == BlockType::SMALL_FLOWER) {
                    packet3 << 4;
                }
                else if (model.getMap().field[i][j] == BlockType::AVERAGE_FLOWER) {
                    packet3 << 5;
                }
                else if (model.getMap().field[i][j] == BlockType::BIG_FLOWER) {
                    packet3 << 6;
                }
            }
        }
        for (int i = 0; i < model.getMap().getRows(); i++) {
            for (int j = 0; j < model.getMap().getCols(); j++) {
                packet3 << model.getMap().score_of_coins[i][j];
            }
        }
        packetWithEnemies.clear();
        packetWithEnemies << "name" << 9 << static_cast<int>(model.getEnemies().size());
        for(auto& enemy : model.getEnemies()){
            if(!enemy->isAlive()){
                continue;
            }
            enemy->currFrame += (time1 / 10'000) * 4.5;
            if(enemy->currFrame >= 4){
                enemy->currFrame = 0;
            }
            //std::cout << "SEND " << enemy->getPos().first << ' ' << enemy->getPos().second << ' ' << enemy->currFrame << "\n";
            packetWithEnemies << enemy->id << enemy->getPos().first << enemy->getPos().second << enemy->hp << enemy->currFrame << static_cast<int>(enemy->getDirection()) << static_cast<int>(enemy->getAction());
            if(enemy->getAction() == Action::ATTACK){
                packetWithEnemies << model.getMap().time_hurt[enemy->getMapPos().first][enemy->getMapPos().second];
            }
        }
        for (auto t = clients.begin(); t != clients.end(); t++) {
            sf::TcpSocket *cl = **t;
            if (selector.isReady(*cl)) {
                (*cl).send(packet3);
                (*cl).send(packetWithEnemies);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        //for (size_t i = 0; i < model.getBullets().size(); i++) {
        //    auto& bullet = *model.getBullets()[i];
        //    if (bullet.life == false) {
        //        model.getBullets().erase(model.getBullets().begin() + i);
        //        packet3.clear();
        //        packet3 << "name" << 8 << static_cast<int>(i);
        //        //std::cout << "deleting " << i << " bullet\n";
        //        for (auto t = clients.begin(); t != clients.end(); t++) {
        //            sf::TcpSocket *cl = **t;
        //            if (selector.isReady(*cl)) {
        //                (*cl).send(packet3);
        //            }
        //        }
        //        i--;
        //        //std::cout << "delet succes\n";
        //    }
        //}

        for (auto u : model.getPlayers()) {
            model.getNames()[u.first] = u.second.life;
        }
    }
}