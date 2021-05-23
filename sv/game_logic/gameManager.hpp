#pragma once
#include <chrono>
#include <vector>

#include "gameManager.hpp"
#include "gameObject.hpp"

class GameManager
{
private:
    /* data */
    std::vector<GameObject *> gameObjList;
    GameState state;

public:
    GameManager(/* args */);
    ~GameManager();
    void add(GameObject *gobj);
    bool update(float deltaTime);
    void start();
    void end();
};
