#include "model.h"
#include "map.h"
#include "vars.h"
#include "bullet.h"
#include <iostream>
#include <string>
#include <random>
Map &Model::getMap() { return map; }
Player &Model::getPlayer() { return plr; }
const std::vector<Button> &Model::getButtons() const { return buttons; }
Model::Model(const std::string &mapFileName) : map(mapFileName), plr("hero.png",100,100,25.0, 41.75), state(GameState::MENU) {
    buttons.emplace_back(L"Одиночная игра", 250, 350);
    buttons.emplace_back(L"Выход", 250, 410);
    buttons.emplace_back(L"Сетевая игра", 250, 380);
    std::cout << "model has " << buttons.size() << "buttons\n";
}
std::mt19937 rng_model(1729); // yes, this is a fixed seed
//std::uniform_real_distribution<> gen_real(0, std::mt19937::max()) ;
std::pair<float, float> Model::randPoint(bool isGreen){
    float rand_x, rand_y;
    while(1){
        rand_x = ((double)rng_model() / std::mt19937::max()) * (map.getCols() * BlockSize);
        rand_y = ((double)rng_model() / std::mt19937::max()) * (map.getRows() * BlockSize);
        BlockType t = map.at(rand_y / BlockSize, rand_x / BlockSize);
        if(!isGreen || (t != BlockType::CONCRETE && t != BlockType::WATER)){
            break;
        }
    }
    return {rand_y, rand_x};
}
void Model::update(float time) {
    time /= 10'000;
    switch (map.at((plr.y + plr.h / 2) / BlockSize, (plr.x + plr.w / 2) / BlockSize))
    {
        case BlockType::WATER:
            plr.health -= (100 * time) / 3;
            break;
        case BlockType::CONCRETE:
            plr.x -= plr.dx * time;
            plr.y -= plr.dy * time;
            break;
        default:
            break;
    }
    for (int col = 0; col < map.getCols(); col++) {
        for (int row = 0; row < map.getRows(); row++) {
            map.time_hurt[row][col] = std::max(static_cast<float>(0), (map.time_hurt[row][col]) - time);
            if (map.field[row][col] == BlockType::SMALL_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 20) {
                    map.life_of_flowers[row][col] += 10* time;
                }
                map.time_of_life[row][col] += 10 * time;
                map.score_of_coins[row][col] += ((map.time_of_life[row][col] - map.last_time[row][col]) / 5);
                if (map.time_of_life[row][col] > 50) {
                    map.score_of_coins[row][col] -= ((map.time_of_life[row][col] - 50) / 5);
                    map.time_of_life[row][col] -= 50;
                    map.last_time[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('5') - 48);
                }
                else {
                    map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
                }
            }
            else if (map.field[row][col] == BlockType::AVERAGE_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 40) {
                    map.life_of_flowers[row][col] += 10 * time;
                }
                map.time_of_life[row][col] += 10 * time;
                map.score_of_coins[row][col] += (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 2);
                if (map.time_of_life[row][col] > 50) {
                    map.score_of_coins[row][col] -= (((map.time_of_life[row][col] - 50) / 5) * 2);
                    map.time_of_life[row][col] -= 50;
                    map.last_time[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('6') - 48);
                }
                else {
                    map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
                }
            }
            else if (map.field[row][col] == BlockType::BIG_FLOWER) {
                if (map.life_of_flowers[row][col] <= 0) {
                    map.life_of_flowers[row][col] = 0;
                    map.field[row][col] = static_cast<BlockType>(static_cast<int>('1') - 48);
                }
                if (map.life_of_flowers[row][col] < 60) {
                    map.life_of_flowers[row][col] += 10 * time;
                }
                map.time_of_life[row][col] += 10 * time;
                map.score_of_coins[row][col] += (((map.time_of_life[row][col] - map.last_time[row][col]) / 5) * 4);
                map.last_time[row][col] = map.time_of_life[row][col] - (map.time_of_life[row][col] % 5);
            }
        }
    }
    if(plr.health <= 0){
        state = GameState::DIED;
        std::cout << "DIIIED\n";
    }
}
std::vector<std::unique_ptr<Enemy>>& ServerModel::getEnemies(){
    return enemies;
}
ServerModel::ServerModel(const std::string& mapFileName, int numOfEnemies) : Model(mapFileName){
    type = 1;
    for(int i = 0; i < numOfEnemies; i++){
        enemies.push_back(std::move(std::unique_ptr<Enemy>(new Zombie(map, i))));
    }   
}
void ServerModel::update(float time){
    Model::update(time);
    for(auto& enemy : enemies){
        enemy->update(time, *this);
    }
}
ClientModel::ClientModel(const std::string& mapFileName) : Model(mapFileName){
    type = 2;
}
std::map<int, ClientEnemy>& ClientModel::getEnemies(){ return enemies; }
bool checkIsPlant(BlockType t){
    return (t == BlockType::AVERAGE_FLOWER || t == BlockType::BIG_FLOWER || t == BlockType::SMALL_FLOWER);
}
std::vector<std::unique_ptr<ServerBullet>>& ServerModel::getBullets(){
    return bullets;
}
std::vector<std::unique_ptr<ClientBullet>>& ClientModel::getBullets(){
    return bullets;
}
std::map<std::string, Player>& Model::getPlayers(){
    return players;
}
std::map<std::string, bool>& Model::getNames(){
    return names;
}