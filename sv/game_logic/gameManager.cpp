#include "gameManager.hpp"

GameManager::GameManager(/* args */)
{
}

GameManager::~GameManager()
{
}

bool GameManager::update(float deltaTime)
{

    for (GameObject *gameobj : gameObjList)
    {
        gameobj->update(deltaTime, state);
        if (state.hasFinish)
        {
            return false;
        }
    }
    //TODO: calculate stop citeria
    return true;
}

void GameManager::start()
{
    // isRunning = true;
    // lastUpdate = std::chrono::high_resolution_clock::now();
    // while (true && isRunning)
    // {
    //     update();
    // }

    // end();
}