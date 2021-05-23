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

class Client
{
private:
    static constexpr int buffer_length = 2;

    int socket;
    struct sockaddr_in server_address;
    char sendline[buffer_length], recvline[buffer_length];
    int bytes_sent, bytes_received;
    /* data */
public:
    Client();
    ~Client();
};
