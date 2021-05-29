#pragma once

#include <memory>
#include <stack>
#include <thread>
#include <vector>

#include "./game/GameServer.hpp"
#include "./utils/ReusableIdList.hpp"
#include "MessageType.hpp"
#include "Room.hpp"
#include "TCPSocket.hpp"

class Server {
   private:
    ReusableIdList<Room> m_roomList;
    int m_bufferSize;
    std::unique_ptr<uint8_t[]> m_buffer;

    TCPSocket m_socket;
    std::vector<std::unique_ptr<game::GameServer>> m_roomVector;

   public:
    Server(uint16_t port = 5000, int maxRoomCount = 4, size_t messageBufferSize = 1024);
    void start();

   private:
    void handleMessage(int sockfd, const uint8_t* buffer, size_t size, const IPEndpoint& client);

    void sendMessage(int sockfd, network::MessageType type, const google::protobuf::Message& message);

    void onQueryRoomInfo(int sockfd, const network::QueryRoomInfoRequest& request);
    void onCreateRoom(int sockfd, const network::CreateRoomRequest& request, const IPEndpoint& client);
    void onJoinRoom(int sockfd, const network::JoinRoomRequest& request, const IPEndpoint& client);
    void onLeaveRoom(int sockfd, const network::LeaveRoomRequest& request, const IPEndpoint& client);
};
