#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
std::string name;
std::string message = "";
sf::Packet packet1, packet2;
sf::TcpListener listener;
sf::SocketSelector selector;
std::mutex m;
std::vector <sf::TcpSocket*> clients;
void F() {
    while (true) {
        for (auto it = clients.begin(); it != clients.end(); it++) {
            sf::TcpSocket& client = **it;
            if (selector.isReady(client)) {
                if (client.receive(packet1) == sf::Socket::Done) {
                    m.lock();
                    std::string nameRec;
                    std::string messageRec;
                    //std::cout << packet.getDataSize() << "\n";
                    packet1 >> nameRec >> messageRec;
                    std::cout << nameRec << ": " << messageRec << '\n';
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
                    clients.push_back(client);
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
    std::cout << "Enter your name:\n";
    std::cin >> name;
    sf::Thread thread2(&F);
    thread2.launch();
    std::string mz = "f";
    while(true) {
        //std::cout << clients.size() << "\n";
        std::cin >> message;
        if (message != "") {
            packet2.clear();
            packet2 << name << message;
            for (auto it = clients.begin(); it != clients.end(); it++) {
                sf::TcpSocket& client = **it;
                if (selector.isReady(client)) {
                    client.send(packet2);
                }
            }
            message = "";
        }
    }
}