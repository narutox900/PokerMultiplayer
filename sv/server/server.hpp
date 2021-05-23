#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "game_logic/gameManager.hpp"

class Server
{
private:
    /* data */
    static constexpr int max_client = 2;
    int listen_socket, connect_socket;
    int current_connection;
    pthread_t client_ids[max_client];
    socklen_t client_length[max_client];
    struct sockaddr_in client_address[max_client], server_address;
    int bytes_sent, bytes_received;
    GameManager game_manager;

public:
    Server(int port_number);
    ~Server();
    void mainLogic();
};
