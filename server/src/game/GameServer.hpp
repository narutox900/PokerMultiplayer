#pragma once

#include <array>
#include <thread>

#include "../../protobuf/src/game.pb.h"
#include "../../protobuf/src/network.pb.h"
#include "../TCPSocket.hpp"
#include "Constant.hpp"
#include "MessageType.hpp"
#include "../IPEndpoint.hpp"
#include "PlayerInfo.hpp"

#define BASEPORT 5000
#define MAXPLAYER 5
namespace game {

class GameServer {
   private:
    size_t m_roomId;
    std::array<PlayerInfo, g_maxPlayerCount> m_playerInfoList;
    static constexpr uint s_bufferSize = 1024;
    std::unique_ptr<uint8_t[]> m_buffer;

    TCPSocket m_socket;
    std::thread m_thread;

   public:
    GameServer(size_t roomID, uint16_t port);
    GameServer();
    bool addPlayer(uint id, const IPEndpoint& endpoint, const int16_t sockfd);
    void start();
    void wait();

   private:
    void sendMessage(const IPEndpoint& receiver, MessageType type, const google::protobuf::Message& message, int sockfd);
    void startGameServer();

    void handleMessage(int sockfd, const uint8_t* buffer, size_t size, const IPEndpoint& client);

};  // namespace game
}
