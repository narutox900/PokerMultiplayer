#include "Client.hpp"
int main() {
    Client client(BASEPORT, BASEADDRESS);
    client.start();
}