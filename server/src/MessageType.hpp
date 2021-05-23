#pragma once

namespace network {

enum class MessageType : uint8_t {
    QueryRoomInfoRequest = 0,
    QueryRoomInfoResponse = 1,

    CreateRoomRequest = 2,
    CreateRoomResponse = 3,

    JoinRoomRequest = 4,
    JoinRoomResponse = 5,

    LeaveRoomRequest = 6,
    LeaveRoomResponse = 7,

    RoomInfoChanged = 8,

    PlayerInput = 9,
    PlayerState = 10,
    GameState = 11
};

}  // namespace network
