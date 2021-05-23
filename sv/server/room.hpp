#pragma once

#include <chrono>
#include <pthread.h>

#include "../game_logic/gameManager.hpp"
#include "client.hpp"

class Room
{
private:
    std::chrono::high_resolution_clock::time_point lastUpdate;
    GameManager gm;
    Client clients[2];
    pthread_t pid;
    uint8_t *buffer;
    bool isRunning;
    static void *threadLoop(void *param);
    void run();

public:
    Room(int socket_c1, int socket_c2);
    ~Room();
    pthread_t startRoom();
};
