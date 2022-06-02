#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include "model.h"
#include "vars.h"
#include "view.h"
#include <iostream>
#include <sstream>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <utility>

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
                if (i <= 0 || j <= 0 || i >= (model.getMap().getRows() - 1) || j >= (model.getMap().getCols() - 1)) {
                    life = false;
                }
                else if (model.getMap().field[i][j] == BlockType::SMALL_FLOWER || model.getMap().field[i][j] == BlockType::AVERAGE_FLOWER || model.getMap().field[i][j] == BlockType::BIG_FLOWER) {
                    life = false;
                    model.getMap().life_of_flowers[i][j] -= 20;
                }
            }
        }
    }
};

std::size_t count;
std::string message = "";
sf::Packet packet1, packet2, packet3;
sf::TcpListener listener;
sf::SocketSelector selector;
std::mutex m;
std::vector <std::unique_ptr<sf::TcpSocket*>> clients;
std::map <std::string, Player> players;
Model model("map.txt");
sf::Clock clock1;
float time1;
std::deque <std::unique_ptr<Bullet>> bullets;

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
                    if (players.find(nameRec) == players.end()) {
                        players.emplace(nameRec, model.getPlayer());
                        if (players.size() <= count) {
                            for (auto name : players) {
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
                        if (players.size() == count) {
                            count = 0;
                        }
                    }

                    packet1 >> command;

                    if (command == 2) {
                        std::string button;
                        packet1 >> button;
                        if (button == "L") {
                            players.at(nameRec).dir = 1; players.at(nameRec).speed = 0.1;
                            players.at(nameRec).CurrentFrame += 0.005*time1;
                            if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                            players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 84, 25, 42));
                            packet2.clear();
                            packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed << players.at(nameRec).CurrentFrame << "L";
                        }
                        else if (button == "R") {
                            players.at(nameRec).dir = 0; players.at(nameRec).speed = 0.1;
                            players.at(nameRec).CurrentFrame += 0.005*time1;
                            if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                            players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 42, 25, 42));
                            packet2.clear();
                            packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed << players.at(nameRec).CurrentFrame << "R";
                        }
                        else if (button == "U") {
                            players.at(nameRec).dir = 3; players.at(nameRec).speed = 0.1;
                            players.at(nameRec).CurrentFrame += 0.005*time1;
                            if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                            players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 0, 25, 42));
                            packet2.clear();
                            packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed << players.at(nameRec).CurrentFrame << "U";
                        }
                        else if (button == "D") {
                            players.at(nameRec).dir = 2; players.at(nameRec).speed = 0.1;
                            players.at(nameRec).CurrentFrame += 0.005*time1;
                            if (players.at(nameRec).CurrentFrame > 4) players.at(nameRec).CurrentFrame -= 4;
                            players.at(nameRec).sprite.setTextureRect(sf::IntRect(25 * int(players.at(nameRec).CurrentFrame), 125, 25, 42));
                            packet2.clear();
                            packet2 << nameRec << 2 << players.at(nameRec).dir << players.at(nameRec).speed << players.at(nameRec).CurrentFrame << "D";
                        }
                        else if (button == "P") {
                            int row = players.at(nameRec).y / 50;
                            int col = players.at(nameRec).x / 50;
                            model.getMap().life_of_flowers[row][col] = 20;
                            model.getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') - 48);
                            packet2.clear();
                            packet2 << nameRec << 3 << row << col;
                        }
                        else if (button == "S") {
                            bullets.emplace_back(std::make_unique<Bullet>("bullet.png",players.at(nameRec).x,players.at(nameRec).y,16.0, 16.0, players.at(nameRec).dir));
                            packet2.clear();
                            packet2 << nameRec << 4;
                        }
                        for (auto t = clients.begin(); t != clients.end(); t++) {
                            sf::TcpSocket *cl = **t;
                            if (selector.isReady(*cl)) {
                                (*cl).send(packet2);
                            }
                        }
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
int main() {
    listener.listen(2000);
    selector.add(listener);
    sf::Thread thread1(&G);
    thread1.launch();
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

        for (auto &u : players) {
            u.second.update(time1, model);
            // std::cout << u.second.x << " " << u.second.y << " " << u.second.speed << " " << u.second.dir << "\n";
            packet3.clear();
            packet3 << u.first << 5 << u.second.x << u.second.y << u.second.leftOnWater << u.second.health << u.second.playerscore;
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
        for (size_t i = 0; i < bullets.size(); i++) {
            (*bullets[i]).update(time1, model);
            packet3.clear();
            packet3 << "name" << 7 << static_cast<int>(i) << (*bullets[i]).x << (*bullets[i]).y << (*bullets[i]).life;
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

        /*for (auto t = clients.begin(); t != clients.end(); t++) {
            sf::TcpSocket *cl = **t;
            if (selector.isReady(*cl)) {
                (*cl).send(packet3);
            }
        }*/

        for (size_t i = 0; i < bullets.size(); i++) {
            if ((*bullets[i]).life == false) {
                bullets.pop_front();
            }
        }
    }


}