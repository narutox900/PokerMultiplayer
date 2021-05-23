#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../protobuf/game.pb.h"

class Client
{
private:
    int socket;

    /* data */
public:
    Client(int connectedSocket);
    ~Client();
    void sendToClient(GameState gs);
    int receiveFromClient(uint8_t *buffer, size_t size);
};
