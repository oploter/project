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
    int dir, id;
    sf::String File;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;
    Bullet(sf::String F, float X, float Y, float W, float H, int dir_, int id_);
    Bullet(){}
};
struct ClientBullet : Bullet{
    ClientBullet(){}
    ClientBullet(sf::String F, float X, float Y, float W, float H, int dir_, int id_);
};
struct ServerBullet : Bullet{
private:    
    static inline const int force = 20;
public:
    std::string name;
    ServerBullet(sf::String F, float X, float Y, float W, float H, int dir_, const std::string &name_, int id_);
    void update(float time, ServerModel &model);
    void interactionWithMap(ServerModel &model);
};
#endif