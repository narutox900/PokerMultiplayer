#include "room.hpp"

Room::Room(int socket_c1, int socket_c2) : clients({socket_c1, socket_c2})
{
    buffer = new uint8_t[1024];
}

Room::~Room()
{
    delete[] buffer;
}

void *Room::threadLoop(void *param)
{
    Room *room = (Room *)param;
    room->run();
}

void Room::run()
{
    while (true && isRunning)
    {
        using clk = std::chrono::high_resolution_clock;
        using namespace std::chrono;
        clk::time_point now = clk::now();
        duration<float> deltaTime = duration_cast<duration<float>>(now - lastUpdate);
        lastUpdate = now;

        for (int i = 0; i < 2; i++)
        {
            int flag = clients[i].receiveFromClient(buffer, 1024);
            if (flag > 0)
            {
                switch (buffer[0])
                {
                case 0: //the direction of client
                    break;
                case 1: //the client is shooting
                    break;
                }
            }
        }
        isRunning = gm.update(deltaTime.count());
    }
}

pthread_t Room::startRoom()
{
    isRunning = true;
    lastUpdate = std::chrono::high_resolution_clock::now();

    pthread_create(&pid, NULL, threadLoop, this);
}