#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "IPEndpoint.hpp"

//TCP Socket Wrapper
class TCPSocket {
   public:
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;

    TCPSocket(TCPSocket&& other) {
        m_socketFd = other.m_socketFd;
        other.m_socketFd = -1;
        m_bindedPort = other.m_bindedPort;
        other.m_bindedPort = -1;
    }
    TCPSocket& operator=(TCPSocket&& other) {
        m_socketFd = other.m_socketFd;
        other.m_socketFd = -1;
        m_bindedPort = other.m_bindedPort;
        other.m_bindedPort = -1;
        return *this;
    };

    TCPSocket() {
        m_bindedPort = -1;
        m_socketFd = -1;
    }

    TCPSocket(uint16_t port, std::string host_addr) : m_bindedPort(0), m_socketFd(-1) {
        // for client
        printf("Connecting to server\n");
        m_bindedPort = port;
        m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socketFd == -1) {
            throw;
        }
        //initialize socket address
        sockaddr_in sockAddr{};
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(port);
        sockAddr.sin_addr.s_addr = inet_addr(host_addr.c_str());

        if (connect(m_socketFd, (struct sockaddr*)&sockAddr, sizeof(sockaddr)) < 0) {
            printf("Error connecting to server\n");
            throw;
        }
        printf("Test %d\n", m_socketFd);
    }

    TCPSocket(uint16_t bindedPort, int maxConnection) {
        printf("initalizing socket\n");
        m_bindedPort = bindedPort;
        m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socketFd < 0) {
            throw;
        }
        // initialize the socket address
        sockaddr_in sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(bindedPort);
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        // bind socket to an address
        int ret = bind(m_socketFd, (sockaddr*)&sockAddr, sizeof(sockAddr));
        if (ret == -1) {
            fprintf(stderr, "Error: bind\n");
            throw;
        }
        // start listening
        listen(maxConnection);  // TODO
    }

    ~TCPSocket() {
        printf("Deconstructing TCP socket %d %d\n", m_socketFd, m_bindedPort);
        if (m_socketFd >= 0) {
            close(m_socketFd);
            m_socketFd = -1;
        }
    }

    int socketFd() const {
        return m_socketFd;
    }

    uint16_t bindedPort() const {
        return m_bindedPort;
    }

    void listen(uint32_t maxConnection) {
        int ret = ::listen(m_socketFd, maxConnection);
        if (ret == -1) {
            throw;
        }
    }

    int accept(struct sockaddr_in* client) {
        socklen_t sin_size = sizeof(struct sockaddr_in);
        int newConFd = ::accept(m_socketFd, (struct sockaddr*)client, &sin_size);
        if (newConFd == -1) {
            throw;
        }
        printf("Accepted %d\n", newConFd);
        return newConFd;
    }

    int send(int sockfd, const uint8_t* buffer, size_t size) {
        return ::send(sockfd, buffer, size, 0);
    }

    int recv(int sockfd, uint8_t* buffer, size_t size) {
        return ::recv(sockfd, buffer, size, 0);
    }

   private:
    uint16_t m_bindedPort;
    int m_socketFd;
};