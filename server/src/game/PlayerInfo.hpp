#include "../IPEndpoint.hpp"

namespace game {
struct PlayerInfo {
    enum class ConnectionState {
        Disconnected = 0,
        Connecting = 1,
        Connected = 2
    };

    int id;
    int32_t balance;
    int status;
    int32_t sockfd;
    IPEndpoint endpoint;
    ConnectionState connectionState;
};
}  // namespace game