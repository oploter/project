#include <SFML/Network.hpp>
#include <iostream>
#include <string>
sf::TcpSocket socket;
std::string name;
std::string message = "";
sf::Packet  packet1;
sf::Packet  packet2;
sf::IpAddress ip;
void F() {
    while (true) {
        if(socket.receive(packet1) == sf::Socket::Done) {
            std::string nameRec;
            std::string messageRec;
            packet1 >> nameRec >> messageRec;
            std::cout << nameRec << ": " << messageRec << '\n';
        }
    }
}
int main() {
    ip = sf::IpAddress::getLocalAddress();
    if(socket.connect(ip, 2000) != sf::Socket::Done) {
        std::cout << "Error!\n";
    }
    std::cout << "Enter your name:\n";
    std::cin  >> name;
    socket.setBlocking(false);
    sf::Thread thread(&F);
    thread.launch();
    while(true) {
        std::cin >> message;
        if(message != "") {
            packet2.clear();
            packet2 << name << message;
            socket.send(packet2);
            message = "";
        }
    }
}