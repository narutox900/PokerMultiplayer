#include "Client.hpp"

#include <google/protobuf/util/json_util.h>
#include <poll.h>
#include <stdio.h>

Client::Client(uint16_t port, std::string hostAddr, size_t messageBufferSize)
    : m_bufferSize(messageBufferSize), m_buffer(new uint8_t[messageBufferSize]), m_socket(port, hostAddr) {}

pollfd pollList[3];

using namespace network;
void printChoiceMenu();
void Client::askForName() {
    char buf[128];

    printf("Enter name: ");
    fgets(buf, 128, stdin);

    m_name = buf;
}

void Client::start() {
    askForName();
    pollList[0] = {.fd = STDIN_FILENO, .events = POLLIN};
    pollList[1] = {.fd = m_socket.socketFd(), .events = POLLIN};
    pollList[2] = {.fd = m_gameSocket.socketFd(), .events = POLLIN};
    int choice = -1;
    printChoiceMenu();

    printf("\nWaiting for events \n");
    while (true) {
        //     printf("Testing\n");
        //     printf("%d\n", m_socket.socketFd());
        //     int choice;
        //     scanf("%d", &choice);
        //     send(m_socket.socketFd(), "test\n", 6, 0);
        //     if (choice == 0) break;
        //     printf("Waiting for event\n");

        poll(pollList, 3, -1);

        if ((pollList[0].revents & POLLIN) != 0) {
            scanf(" %d", &choice);
            printf("%d choice\n", choice);
            switch (choice) {
                case 1: {
                    QueryRoomInfoRequest request{};
                    request.set_not_full(false);

                    printf("Sending QueryRoomInfoRequest\n");
                    sendMessage(m_socket.socketFd(), MessageType::QueryRoomInfoRequest, request);
                    break;
                }
                case 2: {
                    if (m_roomID != -1 || m_clientID != -1) {
                        printf("Already in a room\n");
                        break;
                    }
                    CreateRoomRequest request{};
                    request.mutable_client_info()->set_name(m_name);
                    request.mutable_client_info()->set_id(0);

                    printf("Sending CreateRoomRequest\n");
                    sendMessage(m_socket.socketFd(), MessageType::CreateRoomRequest, request);
                    break;
                }
                case 3: {
                    if (m_roomID != -1 || m_clientID != -1) {
                        printf("Already in a room\n");
                        break;
                    }

                    JoinRoomRequest request{};
                    request.mutable_client_info()->set_name(m_name);
                    request.mutable_client_info()->set_id(0);

                    printf("Enter room id: ");
                    int roomID;
                    scanf(" %d", &roomID);
                    sendMessage(m_socket.socketFd(), MessageType::JoinRoomRequest, request);
                    request.set_room_id(roomID);

                    printf("Sending JoinRoomRequest\n");
                    break;
                }
                case 4: {
                    if (m_roomID == -1 || m_clientID == -1) {
                        printf("Not in a room\n");
                        break;
                    }
                    LeaveRoomRequest request{};
                    request.mutable_client_identity()->set_room_id(m_roomID);
                    request.mutable_client_identity()->set_client_id(m_clientID);

                    printf("Sending LeaveRoomRequest\n");
                    sendMessage(m_socket.socketFd(), MessageType::LeaveRoomRequest, request);
                    break;
                }
                case 5: {
                    if (m_roomID != -1 || m_clientID != -1) {
                        send(m_gameSocket.socketFd(), "test\n", 7, 0);
                    }
                }
                default:
                    break;
            }
        }

        if (pollList[1].revents & POLLIN != 0) {
            ssize_t read = recv(m_socket.socketFd(), m_buffer.get(), sizeof(int), 0);
            printf("Received %ld byte packet from server\n", read);
            int* length = (int*)m_buffer.get();

            read = recv(m_socket.socketFd(), m_buffer.get(), *length + 1, 0);
            printf("receiving %ld bytes\n", read);
            if (read == -1) {
                printf("Socket read error: errorno %d", errno);
                break;
            }

            if (read >= m_bufferSize) {
                continue;
            }

            handleMessage(m_socket.socketFd(), m_buffer.get(), read);
        }

        if (pollList[2].revents & POLLIN != 0) {
            // ssize_t read = recv(m_socket.socketFd(), m_buffer.get(), sizeof(int), 0);
            // printf("Received %ld byte packet from server\n", read);
            // int* length = (int*)m_buffer.get();

            // read = recv(m_socket.socketFd(), m_buffer.get(), *length + 1, 0);
            // printf("receiving %ld bytes\n", read);
            // if (read == -1) {
            //     printf("Socket read error: errorno %d", errno);
            //     break;
            // }

            // if (read >= m_bufferSize) {
            //     continue;
            // }

            // handleMessage(m_socket.socketFd(), m_buffer.get(), read);
            ssize_t read = recv(m_socket.socketFd(), m_buffer.get(), sizeof(int), 0);
            if (read == 0) {
                break;
            }
            printf("Read %ld bytes from game sv\n", read);
        }
    }
}

void printChoiceMenu() {
    printf("******************\n");
    printf("1.Query room info\n");
    printf("2.Create room request\n");
    printf("3. Join room request\n");
    printf("4.Leave room request\n");
    printf("***************\n");
}

void Client::sendMessage(int sockfd, network::MessageType type, const google::protobuf::Message& message) {
    uint8_t* buffer = m_buffer.get();

    // Write message type
    int messageSize = message.ByteSizeLong();
    int* length = (int*)(&buffer[0]);

    *length = messageSize;
    buffer[sizeof(int)] = (uint8_t)type;
    bool ret = message.SerializeToArray(buffer + 1 + sizeof(int), messageSize);
    assert(ret);  //Write message
    m_socket.send(sockfd, buffer, messageSize + 1 + sizeof(int));
}

static void printProtoAsJson(google::protobuf::Message& proto) {
    namespace pu = google::protobuf::util;

    std::string json;
    pu::JsonOptions jsonOptions;
    jsonOptions.always_print_primitive_fields = true;
    jsonOptions.add_whitespace = true;
    pu::MessageToJsonString(proto, &json, jsonOptions);

    printf("%s\n", json.c_str());
}

void Client::handleMessage(int sockfd, const uint8_t* buffer, size_t size) {
    // Read message type

    MessageType mesgType = (MessageType)buffer[0];
    size_t messageSize = size - 1;
    buffer += 1;

    switch (mesgType) {
        case MessageType::QueryRoomInfoResponse: {
            QueryRoomInfoResponse response{};
            bool parseResult = response.ParseFromArray(buffer, messageSize);
            assert(parseResult);

            printf("QueryRoomInfoResponse:\n");

            printProtoAsJson(response);
            break;
        }
        case MessageType::CreateRoomResponse: {
            CreateRoomResponse response{};
            bool parseResult = response.ParseFromArray(buffer, messageSize);
            assert(parseResult);

            if (response.success() == false) {
                printProtoAsJson(response);
                break;
            }

            m_clientID = response.assigned_identity().client_id();
            m_roomID = response.assigned_identity().room_id();

            sleep(1);

            m_gameSocket = TCPSocket(BASEPORT + m_roomID + 1, BASEADDRESS);
            pollList[2] = {.fd = m_gameSocket.socketFd(), .events = POLLIN};

            printf("clientId: %d roomid: %ld\n", m_clientID, m_roomID);
            printf("CreateRoomResponse:\n");

            printProtoAsJson(response);
            break;
        }
        case MessageType::JoinRoomResponse: {
            JoinRoomResponse response{};
            bool parseResult = response.ParseFromArray(buffer, messageSize);
            assert(parseResult);

            if (response.success() == false) {
                printProtoAsJson(response);
                break;
            }
            m_clientID = response.assigned_identity().client_id();
            m_roomID = response.assigned_identity().room_id();
            m_gameSocket = TCPSocket(BASEPORT + m_roomID + 1, BASEADDRESS);
            pollList[2] = {.fd = m_gameSocket.socketFd(), .events = POLLIN};
            printf("JoinRoomResponse");
            printProtoAsJson(response);
            break;
        }
        case MessageType::LeaveRoomResponse: {
            LeaveRoomResponse response{};
            bool parseResult = response.ParseFromArray(buffer, messageSize);
            assert(parseResult);
            if (response.success() == false) {
                printProtoAsJson(response);
                break;
            }

            m_clientID = -1;
            m_roomID = -1;
            m_gameSocket = TCPSocket();
            pollList[2] = {.fd = m_gameSocket.socketFd(), .events = POLLIN};

            printf("LeaveRoomResponse:\n");

            printProtoAsJson(response);
            break;
        }
        case MessageType::RoomInfoChanged: {
            network::RoomInfoChanged response{};
            bool parseResult = response.ParseFromArray(buffer, messageSize);
            assert(parseResult);

            printf("RoomInfoChanged:\n");

            printProtoAsJson(response);
            break;
        }
        default:
            break;
    }
}