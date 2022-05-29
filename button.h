#ifndef BUTTON_H
#define BUTTON_H
#include <SFML/Graphics.hpp>
#include <string>
#include <utility>
struct Button {
private:
  std::wstring label;
  int x, y, w;
  int h = 30;

public:
  Button(std::wstring label_, int x_, int y_)
      : label(std::move(label_)), x(x_), y(y_), w(22 * label.size()) {}
  bool isClicked(int mouse_x, int mouse_y) const {
    return (x <= mouse_x && mouse_x <= x + w && y <= mouse_y &&
            mouse_y <= y + h);
  }
  std::pair<int, int> coordinates() const { return {x, y}; }
  std::pair<int, int> dimensions() const { return {w, h}; };
  const std::wstring &getLabel() const { return label; }
};
#endif