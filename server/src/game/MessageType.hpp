#pragma once
namespace game {
enum class MessageType : uint8_t {
    StartGameRequest = 0,
    StartGameResponse = 1,
    DealCards = 2,
    DealCommunityCards = 3,
    BetTurn = 4,
    BetTurnResponse =5,
    DoneBet = 6,
    EndRound = 7,
    Result = 8
};
}