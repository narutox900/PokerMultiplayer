#include "Server.hpp"

#include <poll.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
#include <thread>

#define MAXCONNECTION 20
#define MAXROOM 4

Server::Server(uint16_t port, int maxRoomCount, size_t messageBufferSize)
    : m_socket(port, MAXCONNECTION), m_roomList(maxRoomCount), m_bufferSize(messageBufferSize), m_buffer(new uint8_t[messageBufferSize]) {}

void Server::start() {
    int nready, i, maxi, port, listenfd, connfd, sockfd;
    struct sockaddr_in cliaddr, servaddr;
    const int OPEN_MAX = sysconf(_SC_OPEN_MAX);  // maximum number of opened files
    struct pollfd clients[OPEN_MAX];
    // IPEndpoint clients_endpoint[OPEN_MAX];
    struct sockaddr_in clients_addr[OPEN_MAX];
    ssize_t n;
    int INFTIM = -1;
    maxi = 20;  // max index into the clients[] array

    // The server socket
    clients[0].fd = m_socket.socketFd();
    clients[0].events = POLLRDNORM;

    for (i = 1; i < OPEN_MAX; i++) {
        clients[i].fd = -1;  //avaiable entries
    }

    while (1) {
        nready = poll(clients, maxi + 1, INFTIM);

        if (nready <= 0) {
            continue;
        }

        // Check for new connection
        if (clients[0].revents & POLLRDNORM) {
            connfd = m_socket.accept(&cliaddr);
            if (connfd < 0) {
                assert("connfd < 0");
                throw;
            }

            printf("Accept socket %d (%s : %hu)\n",
                   connfd,
                   inet_ntoa(cliaddr.sin_addr),
                   ntohs(cliaddr.sin_port));

            // Save client socket into clients array
            for (i = 0; i < OPEN_MAX; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = connfd;

                    clients_addr[i] = cliaddr;
                    break;
                }
            }

            // No enough space in clients array
            if (i == OPEN_MAX) {
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
                n = recv(sockfd, m_buffer.get(), m_bufferSize, 0);
                if (n == -1) {
                    throw;
                }
                if (n == 0) {
                    close(clients[i].fd);
                    clients[i].fd = -1;
                }
                IPEndpoint client{clients_addr[i]};
                printf("Received %ld byte packet: %s:%d buffer\n", n, client.addressAsStr(), client.port() );
                handleMessage(sockfd, m_buffer.get(), n, client);
            }
        }
    }
}

void Server::handleMessage(int sockfd, const uint8_t* buffer, size_t size, const IPEndpoint& client) {
    network::MessageType mesgType = (network::MessageType)buffer[0];
    size_t messageSize = size - 1;
    buffer += 1;
    switch (mesgType) {
        case network::MessageType::CreateRoomRequest: {
            /* code */
            network::CreateRoomRequest request;
            bool parseResult = request.ParseFromArray(buffer, messageSize);
            assert(parseResult);
            onCreateRoom(sockfd, request, client);
            break;
        }
        case network::MessageType::QueryRoomInfoRequest: {
            network::QueryRoomInfoRequest request;
            bool parseResult = request.ParseFromArray(buffer, messageSize);
            assert(parseResult);
            onQueryRoomInfo(sockfd, request);
            break;
        }
        case network::MessageType::JoinRoomRequest: {
            network::JoinRoomRequest request;
            bool parseResult = request.ParseFromArray(buffer, messageSize);
            assert(parseResult);
            onJoinRoom(sockfd, request, client);
            break;
        }
        case network::MessageType::LeaveRoomRequest: {
            network::LeaveRoomRequest request;
            bool parseResult = request.ParseFromArray(buffer, messageSize);
            assert(parseResult);
            onLeaveRoom(sockfd, request, client);
            break;
        }

        default:
            break;
    }
}

void Server::sendMessage(int sockfd, network::MessageType type, const google::protobuf::Message& message) {
    uint8_t* buffer = m_buffer.get();

    //Set message type

    // Write message
    int messageSize = message.ByteSizeLong();
    int* length = (int*)(&buffer[0]);
    *length = messageSize;
    buffer[sizeof(int)] = (uint8_t)type;
    bool ret = message.SerializeToArray(buffer + 1 + sizeof(int), messageSize);
    assert(ret);

    send(sockfd, buffer, messageSize + 1 + sizeof(int), 0);
}

void Server::onQueryRoomInfo(int sockfd, const network::QueryRoomInfoRequest& request) {
    network::QueryRoomInfoResponse response;

    for (const auto& [id, room] : m_roomList) {
        if (request.not_full() && room.is_full()) {
            continue;
        }

        network::RoomInfo* roomInfo = response.add_rooms();
        roomInfo->set_id(id);
        room.serializeToRoomInfo(*roomInfo);
    }

    sendMessage(sockfd, network::MessageType::QueryRoomInfoResponse, response);
}

void Server::onCreateRoom(int sockfd, const network::CreateRoomRequest& request, const IPEndpoint& client) {
    network::CreateRoomResponse response{};

    const auto [roomIter, addRoomResult] = m_roomList.add(Room());

    if (!addRoomResult) {
        response.set_success(false);
        sendMessage(sockfd, network::MessageType::CreateRoomResponse, response);
        return;
    }

    auto& [roomID, newRoom] = *roomIter;

    Player player{request.client_info().name(), client, sockfd};
    int clientID = newRoom.addPlayer(std::move(player));

    if (clientID < 0) {
        response.set_success(false);
        sendMessage(sockfd, network::MessageType::CreateRoomResponse, response);
        return;
    }

    printf("Creating room: roomID %d clientID %d\n",roomID, clientID);
    auto& room = m_roomVector.emplace_back(std::make_unique<game::GameServer>(roomID,BASEPORT+roomID+1));
    room.get()->start();
    response.set_success(true);
    network::ClientIdentity* clientIdentity = response.mutable_assigned_identity();
    clientIdentity->set_room_id(roomID);
    clientIdentity->set_client_id(clientID);

    sendMessage(sockfd, network::MessageType::CreateRoomResponse, response);

    //Broadcasting roomInfoChanged message
    network::RoomInfoChanged roomInfoChangedMessage{};
    newRoom.serializeToRoomInfo(*roomInfoChangedMessage.mutable_new_room_info());

    for (const auto& [playerID, player] : newRoom) {
        sendMessage(player.sockfd(), network::MessageType::RoomInfoChanged, roomInfoChangedMessage);
    }

}
void Server::onJoinRoom(int sockfd, const network::JoinRoomRequest& request, const IPEndpoint& client) {
    network::JoinRoomResponse response{};

    size_t roomID = request.room_id();
    Room* room = m_roomList.get(roomID);

    // not found
    if (room == nullptr) {
        response.set_success(false);
        sendMessage(sockfd, network::MessageType::JoinRoomResponse, response);
        return;
    }

    // Add player to room
    Player player{request.client_info().name(), client, sockfd};
    int clientID = room->addPlayer(std::move(player));

    // add unsuccessfully
    if (clientID < 0) {
        response.set_success(false);
        sendMessage(sockfd, network::MessageType::JoinRoomResponse, response);
        return;
    }

    response.set_success(true);
    network::ClientIdentity* clientIdentity = response.mutable_assigned_identity();
    clientIdentity->set_client_id(clientID);
    clientIdentity->set_room_id(roomID);

    sendMessage(sockfd, network::MessageType::JoinRoomResponse, response);

    // Broadcast to others in room
    network::RoomInfoChanged roomInfoChangedMessage{};
    room->serializeToRoomInfo(*roomInfoChangedMessage.mutable_new_room_info());

    for (const auto& [playerID, player] : *room) {
        sendMessage(player.sockfd(), network::MessageType::RoomInfoChanged, roomInfoChangedMessage);
    }
    return;
}
void Server::onLeaveRoom(int sockfd, const network::LeaveRoomRequest& request, const IPEndpoint& client) {
    network::LeaveRoomResponse response;

    size_t roomID = request.client_identity().room_id();
    Room* room = m_roomList.get(roomID);

    // cant find room
    if (room == nullptr) {
        response.set_success(false);
        sendMessage(sockfd, network::MessageType::LeaveRoomResponse, response);
        return;
    }

    uint clientID = request.client_identity().client_id();
    const Player* player = room->findPlayer(clientID);

    // cant find player in room
    if (player == nullptr) {
        response.set_success(false);
        sendMessage(sockfd, network::MessageType::LeaveRoomResponse, response);
        return;
    }

    room->removePlayer(clientID);

    response.set_success(true);
    sendMessage(sockfd, network::MessageType::LeaveRoomResponse, response);

    //Broadcast to others in room
    network::RoomInfoChanged roomInfoChangedMessage{};
    room->serializeToRoomInfo(*roomInfoChangedMessage.mutable_new_room_info());

    for (const auto& [playerID, player] : *room) {
        sendMessage(player.sockfd(), network::MessageType::RoomInfoChanged, roomInfoChangedMessage);
    }
    return;
}
