#ifndef BULLET_H
#define BULLET_H
#include <SFML/Graphics.hpp>
#include <string>
struct Model;
struct ServerModel;
struct Bullet{
    virtual ~Bullet() = default;
    float x, y;
    float w, h, dx, dy, speed;
    bool life;
    int dir;
    sf::String File;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    Bullet(sf::String F, float X, float Y, float W, float H, int dir_);

};
struct ClientBullet : Bullet{
    ClientBullet(sf::String F, float X, float Y, float W, float H, int dir_);
};
struct ServerBullet : Bullet{
private:    
    const int force = 20;
public:
    std::string name;
    ServerBullet(sf::String F, float X, float Y, float W, float H, int dir_, const std::string &name_);
    void update(float time, ServerModel &model);
    void interactionWithMap(ServerModel &model);
};
#endif