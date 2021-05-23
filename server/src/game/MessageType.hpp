#pragma once
namespace game {
enum class MessageType : uint8_t {
    StartGame = 0,
    DealCards = 1,
    DealCommunityCards = 2,
    BetTurn = 3,
    ClientBet = 4,
    DoneBet = 5,
    EndRound = 6,
    Result = 7
};
}