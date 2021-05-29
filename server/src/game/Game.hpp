#pragma once
#include <array>
#include <vector>

#include "Deck.hpp"
#include "PlayerInfo.hpp"
#include "array"
namespace game {

struct Player {
    Card cards[2];
};

enum status {
    NOT_PLAYING = -1,
    PLAYING = 1
};

class Game {
   private:
    Deck m_deck;
    std::array<Card, 5> m_communityCards;
    int m_currentBet;
    int m_pool;
    std::array<PlayerInfo,5> m_playerInfoList;
    std::array<Player,5> playerHands;

   public:
    Game();
    Game(std::array<PlayerInfo, 5> &playerInfoList);
    Card* dealPlayerCards();
    void dealCommunityCard();
    void foldPlayer(int id);
    void callPlayer(int id);
    void raisePlayer(int id, int amount);


   private:
    int calculatePoint(int id);
    int isStraightFlush(Card* cards);  //*100000
    int isFourOfAKind(Card* cards);    // *50000
    int isFullHouse(Card* cards);      // *10000
    int isFlush(Card* cards);          // *5000
    int isStraight(Card* cards);       // * 1000
    int isThreeOfAKind(Card* cards);   //* 500
    int isTwoPair(Card* cards);        // 100
    int isPair(Card* cards);           // 50
    Card* sortCardsByValue(Card* cards);
    Card* sortCardsBySuit(Card* cards);
};

}  // namespace game
