#pragma once

#include <memory>
#include <string>

#include "../../protobuf/src/network.pb.h"
#include "../../server/src/IPEndpoint.hpp"
#include "../../server/src/MessageType.hpp"
#include "../../server/src/TCPSocket.hpp"
#define BASEPORT 5000
#define BASEADDRESS "127.0.0.1"

class Client {
   private:
    int m_bufferSize;
    std::unique_ptr<uint8_t[]> m_buffer;

    std::string m_name;
    ssize_t m_roomID = -1;
    int m_clientID = -1;
    TCPSocket m_socket;
    TCPSocket m_gameSocket;

   public:
    Client(uint16_t port, std::string hostAddr, size_t messageBufferSize = 1024);
    void start();

   private:
    void handleMessage(int sockfd, const uint8_t* buffer, size_t size);
    void sendMessage(int sockfd, network::MessageType type, const google::protobuf::Message& message);
    void askForName();
};