#include "model.h"
#include "vars.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player { // класс Игрока
private:
    float x, y;
public:
    float w, h, dx, dy, speed, health, ability, ind; //координаты игрока х и у, высота ширина, ускорение (по х и по у), сама скорость
    bool life;
    int dir, playerscore; //направление (direction) движения игрока
    sf::String File; //файл с расширением
    sf::Image image;//сфмл изображение
    sf::Texture texture;//сфмл текстура
    sf::Sprite sprite;//сфмл спрайт

    Player(sf::String F, float X, float Y, float W, float H){ //Конструктор с параметрами(формальными) для класса Player. При создании объекта класса мы будем задавать имя файла, координату Х и У, ширину и высоту
        playerscore = 0;
        health = 100;
        ability = 50;
        ind = 0;
        life = true;
        dx=0;dy=0;speed=0;dir=0;
        File = F;//имя файла+расширение
        w = W; h = H;//высота и ширина
        image.loadFromFile("../img/" + File);//запихиваем в image наше изображение вместо File мы передадим то, что пропишем при создании объекта. В нашем случае "hero.png" и получится запись идентичная image.loadFromFile("images/hero/png");
        //image.createMaskFromColor(sf::Color(255, 192, 203));//убираем ненужный темно-синий цвет, эта тень мне показалась не красивой.
        texture.loadFromImage(image);//закидываем наше изображение в текстуру
        sprite.setTexture(texture);//заливаем спрайт текстурой
        x = X; y = Y;//координата появления спрайта
        sprite.setTextureRect(sf::IntRect(0, 0, w, h)); //Задаем спрайту один прямоугольник для вывода одного льва, а не кучи львов сразу. IntRect - приведение типов
    }

    void update(float time, Model &model) //функция "оживления" объекта класса. update - обновление. принимает в себя время SFML , вследствие чего работает бесконечно, давая персонажу движение.
    {
        switch (dir)//реализуем поведение в зависимости от направления. (каждая цифра соответствует направлению)
        {
            case 0: dx = speed; dy = 0; break;//по иксу задаем положительную скорость, по игреку зануляем. получаем, что персонаж идет только вправо
            case 1: dx = -speed; dy = 0; break;//по иксу задаем отрицательную скорость, по игреку зануляем. получается, что персонаж идет только влево
            case 2: dx = 0; dy = speed; break;//по иксу задаем нулевое значение, по игреку положительное. получается, что персонаж идет только вниз
            case 3: dx = 0; dy = -speed; break;//по иксу задаем нулевое значение, по игреку отрицательное. получается, что персонаж идет только вверх
        }

        x += dx*time;//то движение из прошлого урока. наше ускорение на время получаем смещение координат и как следствие движение
        y += dy*time;//аналогично по игреку

        speed = 0;//зануляем скорость, чтобы персонаж остановился.
        sprite.setPosition(x,y); //выводим спрайт в позицию x y , посередине. бесконечно выводим в этой функции, иначе бы наш спрайт стоял на месте.
        interactionWithMap(model);
    }

    void interactionWithMap(Model &model) {
        for (int i = y / 50; i < (y + h) / 50; i++) {
            for (int j = x / 50; j < (x + w) / 50; j++) {
                if (model.getMap().field[i][j] == BlockType::WATER) {
                    health = 0;
                }
                if (model.getMap().field[i][j] == BlockType::MOLE) {
                    health -= 0.001;
                }
                playerscore += model.getMap().score_of_coins[i][j];
                model.getMap().score_of_coins[i][j] = 0;
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


int main() {
  sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
  Model model("../map.txt");
  View view(window, model);
  GameState state = GameState::MENU;

  Player p("hero.png",0,0,25.0, 41.75);//создаем объект p класса player,задаем "hero.png" как имя файла+расширение, далее координата Х,У, ширина, высота.

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
          switch (state)
          {
          case GameState::MENU:
              if(ev.type == sf::Event::MouseButtonPressed){
                auto mousePos = sf::Mouse::getPosition(window);
                if(model.getButtons()[0].isClicked(mousePos.x, mousePos.y)){ // нажали "Single game"
                  state = GameState::GAME;
                }else if(model.getButtons()[1].isClicked(mousePos.x, mousePos.y)){ // нажали "Exit"
                  window.close();
                }
              }
              break;
          case GameState::GAME:
            // тут проверяем нажатие клавиш в самой игре
            break;
          }
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left))) { //если нажата клавиша стрелка влево или англ буква А
          p.dir = 1; p.speed = 0.1;//dir =1 - направление вверх, speed =0.1 - скорость движения. Заметьте - время мы уже здесь ни на что не умножаем и нигде не используем каждый раз
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 84, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) {
          p.dir = 0; p.speed = 0.1;//направление вправо, см выше
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 42, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
          p.dir = 3; p.speed = 0.1;//направление вниз, см выше
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 0, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down))) {
          p.dir = 2; p.speed = 0.1;//направление вверх, см выше
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 4) CurrentFrame -= 4; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 125, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::P))) {
          int row = p.getY() / 50;
          int col = p.getX() / 50;
          if (p.ability >= 50 && model.getMap().field[row][col] == BlockType::GREEN) {
              model.getMap().field[row][col] = static_cast<BlockType>(static_cast<int>('4') - 48);
              p.ability = 0;
          }
          else {
              p.ind = 1;
              //std::cout << "До следующей возможности посадить цветок осталось " << 50 - p.ability << " секунд" << "\n";
          }
      }

      std::cout << model.getMap().time_of_life[0][0] << "\n";

      p.update(time, model);//оживляем объект p класса Player с помощью времени sfml, передавая время в качестве параметра функции update. благодаря этому персонаж может двигаться
      model.update(time);
      p.ability += 0.001 * time;

      window.clear();
      switch (state)
      {
      case GameState::MENU:
          view.drawMenu();
          break;
      case GameState::GAME:
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
          window.draw(p.sprite);//рисуем спрайт объекта p класса player

          if (p.ind == 1) {
              std::wostringstream M;
              M << (50 - p.ability);
              text_m.setString(L"Осталось " + M.str() + L" секунд");
              text_m.setPosition(450, 0);
              window.draw(text_m);
              p.ind = 0;
          }
          break;
      }
      window.display();
  }

    return 0;
}
