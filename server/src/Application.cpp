#include "Server.hpp"

int main() {
    constexpr uint16_t port = 5000;
    constexpr int maxRoomCount = 4;

    printf("starting\n");
    Server server(port, maxRoomCount, 1024);
    server.start();
}