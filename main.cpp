#include "model.h"
#include "view.h"
#include <SFML/Graphics.hpp>
#include <iostream>

////////////////////////////////////////////////////КЛАСС ИГРОКА////////////////////////
class Player { // класс Игрока
public:
    float x, y, w, h, dx, dy, speed ; //координаты игрока х и у, высота ширина, ускорение (по х и по у), сама скорость
    int dir ; //направление (direction) движения игрока
    sf::String File; //файл с расширением
    sf::Image image;//сфмл изображение
    sf::Texture texture;//сфмл текстура
    sf::Sprite sprite;//сфмл спрайт

    Player(sf::String F, float X, float Y, float W, float H){ //Конструктор с параметрами(формальными) для класса Player. При создании объекта класса мы будем задавать имя файла, координату Х и У, ширину и высоту
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

    void update(float time) //функция "оживления" объекта класса. update - обновление. принимает в себя время SFML , вследствие чего работает бесконечно, давая персонажу движение.
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
    }
};


int main() {
<<<<<<< HEAD
    sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
    window.setFramerateLimit(30);
    Model model("../map.txt");
    View view(window, model);
    Player plr(0);
    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        view.drawMap();
        view.drawPlayer(plr);
        window.display();
    }
}
=======
  sf::RenderWindow window(sf::VideoMode(750, 750), "Window");
  Model model("../map.txt");
  View view(window, model);

  Player p("hero.png",0,0,25.0, 41.75);//создаем объект p класса player,задаем "hero.png" как имя файла+расширение, далее координата Х,У, ширина, высота.

  float CurrentFrame = 0;//хранит текущий кадр
  sf::Clock clock;

  while (window.isOpen()) {
      float time = clock.getElapsedTime().asMicroseconds();
      clock.restart();
      time = time / 500;
      sf::Event ev;

      while (window.pollEvent(ev)) {
          if (ev.type == sf::Event::Closed) {
              window.close();
          }
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left))) { //если нажата клавиша стрелка влево или англ буква А
          p.dir = 1; p.speed = 0.1;//dir =1 - направление вверх, speed =0.1 - скорость движения. Заметьте - время мы уже здесь ни на что не умножаем и нигде не используем каждый раз
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 3) CurrentFrame -= 3; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 84, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) {
          p.dir = 0; p.speed = 0.1;//направление вправо, см выше
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 3) CurrentFrame -= 3; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 42, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
          p.dir = 3; p.speed = 0.1;//направление вниз, см выше
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 3) CurrentFrame -= 3; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 0, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down))) {
          p.dir = 2; p.speed = 0.1;//направление вверх, см выше
          CurrentFrame += 0.005*time; //служит для прохождения по "кадрам". переменная доходит до трех суммируя произведение времени и скорости. изменив 0.005 можно изменить скорость анимации
          if (CurrentFrame > 3) CurrentFrame -= 3; //проходимся по кадрам с первого по третий включительно. если пришли к третьему кадру - откидываемся назад.
          p.sprite.setTextureRect(sf::IntRect(25 * int(CurrentFrame), 125, 25, 42)); //проходимся по координатам Х. получается 96,96*2,96*3 и опять 96
      }

      p.update(time);//оживляем объект p класса Player с помощью времени sfml, передавая время в качестве параметра функции update. благодаря этому персонаж может двигаться


      window.clear();
      view.drawMap();
      window.draw(p.sprite);//рисуем спрайт объекта p класса player
      window.display();
  }

    return 0;
}

>>>>>>> 498ed3cb0f2ff3ef47c0dd97514efe6fbf6afc63
