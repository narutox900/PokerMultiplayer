#include "client.hpp"

Client::Client()
{
    memset(&server_address, 0, sizeof(server_address));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr();
    servaddr.sin_port = htons(port_number);
}

Client::~Client()
{
}
