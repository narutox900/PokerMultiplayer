#pragma once
#include <array>
#include <vector>

#include "Constant.hpp"
#include "Deck.hpp"
#include "PlayerInfo.hpp"
#include "array"
namespace game {

struct Player {
    Card cards[2];
};

enum status : int {
    NOT_PLAYING = -1,
    PLAYING = 1
};

enum action : int {
    CALL = 0,
    BET = 1,
    FOLD = 2
};

class Game {
   private:
    Deck m_deck;

   public:
    std::array<Card, 5> m_communityCards;
    int m_currentBet;
    int m_endTurnID;
    int m_activePlayerCount;
    int m_pool;
    std::array<PlayerInfo, 5> m_playerInfoList;
    std::array<Player, 5> playerHands;

   public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&& other) {
        m_deck = other.m_deck;
        m_communityCards = other.m_communityCards;
        m_currentBet = other.m_currentBet;
        m_endTurnID = other.m_endTurnID;
        m_pool = other.m_pool;
        m_playerInfoList = other.m_playerInfoList;
        playerHands = other.playerHands;
        m_activePlayerCount = other.m_activePlayerCount;
    }
    Game& operator=(Game&& other) {
        m_deck = other.m_deck;
        m_communityCards = other.m_communityCards;
        m_currentBet = other.m_currentBet;
        m_endTurnID = other.m_endTurnID;
        m_pool = other.m_pool;
        m_playerInfoList = other.m_playerInfoList;
        playerHands = other.playerHands;
        m_activePlayerCount = other.m_activePlayerCount;
        return *this;
    }

    Game() {
        m_currentBet = -1;
        m_endTurnID = -1;
        m_pool = -1;
    };
    Game(std::array<PlayerInfo, 5>& playerInfoList);
    void dealPlayerCards(int id);
    Card dealCommunityCard();
    void foldPlayer(int id);
    void callPlayer(int id);
    void raisePlayer(int id, int amount);
    int getResult();

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
