#include "model.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <iostream>
int main() {
  sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
  Model model("../map.txt");
  View view(window, model);
  while (window.isOpen()) {
    sf::Event ev;
    while (window.pollEvent(ev)) {
      if (ev.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.clear();
    view.drawMap();
    window.display();
  }
}