#include "client.hpp"

Client::Client(int connectedSocket)
{
    socket = connectedSocket;
}

Client::~Client()
{
}

void Client::sendToClient(GameState gs)
{
    uint8_t buffer[256];
    size_t size = gs.ByteSizeLong();
    gs.SerializeToArray(buffer, size);

    send(socket, buffer, size, 0);
}

int Client::receiveFromClient(uint8_t *buffer, size_t size)
{
    size_t received_size = recv(socket, buffer, size, MSG_DONTWAIT);

    return received_size;
}