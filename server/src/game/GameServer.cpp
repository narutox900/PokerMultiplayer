#include "GameServer.hpp"

#include <poll.h>

namespace game {

GameServer::GameServer(size_t roomID, uint16_t port)
    : m_roomId(roomID), m_socket(port, g_maxPlayerCount), m_playerInfoList{}, m_buffer(new uint8_t[s_bufferSize]) {
    for (auto& player : m_playerInfoList) {
        player.id = -1;
    }
}

bool GameServer::addPlayer(uint id, const IPEndpoint& endpoint, const int16_t sockfd) {
    if (id >= g_maxPlayerCount) {
        return false;
    }
    m_playerInfoList[id].id = id;
    m_playerInfoList[id].endpoint = endpoint;
    m_playerInfoList[id].sockfd = sockfd;
    m_playerInfoList[id].connectionState = PlayerInfo::ConnectionState::Connected;

    return true;
}

void GameServer::start() {
    m_thread = std::thread(&GameServer::startGameServer, this);  // TODO handler
}

void GameServer::wait() {
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void GameServer::startGameServer() {
    int nready, i, maxi, port,  connfd, sockfd;
    struct sockaddr_in cliaddr, servaddr;
    const int OPEN_MAX = sysconf(_SC_OPEN_MAX);  // maximum number of opened files
    struct pollfd clients[OPEN_MAX];
    // IPEndpoint clients_endpoint[OPEN_MAX];
    struct sockaddr_in clients_addr[OPEN_MAX];
    ssize_t n;
    int INFTIM = -1;
    maxi = g_maxPlayerCount;  // max index into the clients[] array

    // The server socket
    clients[0].fd = m_socket.socketFd();
    clients[0].events = POLLRDNORM;

    printf("GameServer %ld start polling\n", m_roomId);

    for (i = 1; i < OPEN_MAX; i++) {
        clients[i].fd = -1;  //avaiable entries
    }

    while (1) {
        nready = poll(clients, maxi + 1, INFTIM);

        printf("While loop\n");
        if (nready <= 0) {
            continue;
        }

        // Check for new connection
        if (clients[0].revents & POLLRDNORM) {
            connfd = m_socket.accept(&cliaddr);
            if (connfd < 0) {
                printf("ERROR connfd <0 at gameserver\n");
                throw;
            }

            printf("Accept game server socket %d (%s : %hu)\n",
                   connfd,
                   inet_ntoa(cliaddr.sin_addr),
                   ntohs(cliaddr.sin_port));

            // Save client socket into clients array
            for (i = 0; i < maxi+1; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = connfd;

                    clients_addr[i] = cliaddr;
                    break;
                }
            }

            // No enough space in clients array
            if (i == maxi+i) {
                fprintf(stderr, "Error: too many clients\n");
                close(connfd);
            }

            clients[i].events = POLLRDNORM;

            if (i > maxi) {
                maxi = i;
            }

            // No more readable file descriptors
            if (--nready <= 0) {
                continue;
            }
        }

        for (i = 1; i <= maxi; i++) {
            if ((sockfd = clients[i].fd) < 0) {
                continue;
            }
            // If the client is readable or errors occur
            if (clients[i].revents & (POLLRDNORM | POLLERR)) {
                //TODO handler
                n = recv(sockfd, m_buffer.get(), s_bufferSize, 0);
                if (n == -1) {
                    throw;
                }
                if (n == 0) {
                    close(clients[i].fd);
                    clients[i].fd = -1;
                }
                IPEndpoint client{clients_addr[i]};
                printf("Received %ld byte packet: %s:%d\n", n, client.addressAsStr(), client.port());
                // handleMessage(sockfd, m_buffer.get(), n, client);
            }
        }
    }
}

void GameServer::handleMessage(int sockfd, const uint8_t* buffer, size_t size, const IPEndpoint& client) {
}
void GameServer::sendMessage(const IPEndpoint& receiver, game::MessageType type, const google::protobuf::Message& message, int sockfd) {
    uint8_t* buffer = m_buffer.get();

    // write type into buffer
    buffer[0] = (uint8_t)type;

    //write message
    size_t messageSize = message.ByteSizeLong();
    bool ret = message.SerializeToArray(buffer + 1, messageSize);
    assert(ret);

    send(sockfd, buffer, messageSize, 0);
}

}  // namespace game
