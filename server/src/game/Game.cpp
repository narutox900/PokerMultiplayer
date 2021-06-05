#include "Game.hpp"

namespace game {

Game::Game(std::array<PlayerInfo, 5>& playerInfoList) : m_deck(), m_currentBet(0), m_pool(0), m_endTurnID(-1), m_activePlayerCount(0) {
    m_playerInfoList = &playerInfoList;
    for (int i = 0; i < 5; ++i) {
        m_communityCards[i].value = -1;
    }
};

void Game::dealPlayerCards(int id) {
    playerHands[id].cards[0] = m_deck.deal();
    playerHands[id].cards[1] = m_deck.deal();
}

Card Game::dealCommunityCard() {
    for (int i = 0; i < 5; ++i) {
        if (m_communityCards[i].value == -1) {
            m_communityCards[i] = m_deck.deal();
            return m_communityCards[i];
        }
    }
    Card tmp;
    tmp.value = -1;
    tmp.suit = -1;
    return tmp;
}
void Game::foldPlayer(int id) {
    (*m_playerInfoList)[id].status = NOT_PLAYING;
    m_activePlayerCount--;
}
void Game::callPlayer(int id) {
    (*m_playerInfoList)[id].balance -= m_currentBet - (*m_playerInfoList)[id].currentBet;
    m_pool += m_currentBet - (*m_playerInfoList)[id].currentBet;
    (*m_playerInfoList)[id].currentBet = m_currentBet;
}
void Game::raisePlayer(int id, int amount) {
    (*m_playerInfoList)[id].balance -= amount;
    (*m_playerInfoList)[id].currentBet += amount;
    m_currentBet = (*m_playerInfoList)[id].currentBet;
    m_pool += amount;
    m_endTurnID = id;
}

int Game::isStraightFlush(Card* cards) {
    int count = 0;
    for (int i = 0; i < 7 - 1; i++) {
        if ((cards[i].value == cards[i + 1].value + 1) && (cards[i].suit == cards[i + 1].suit)) {
            count++;
            if (count == 4) {
                return 100000 + cards[i].value + 4;
            }
        } else {
            count = 0;
        }
    }
    return 0;
}

int Game::isFourOfAKind(Card* cards) {
    int count = 0;
    for (int i = 0; i < 7 - 1; i++) {
        if (cards[i].value == cards[i + 1].value) {
            count++;
            if (count == 3) {
                return 50000 + cards[i].value;
            }
        } else {
            count = 0;
        }
    }
    return 0;
}

int Game::isFullHouse(Card* cards) {
    int countThree = 0;
    int countTwo = 0;

    // Check if there is 3 of a kind
    for (int i = 0; i < 7 - 1; i++) {
        if (cards[i].value == cards[i + 1].value) {
            countThree++;
            if (countThree == 2) {
                for (int j = 0; j < i - 1; j++) {
                    if (cards[j].value == cards[j + 1].value) {
                        countTwo++;
                        if (countTwo == 1) {
                            return 10000 + cards[i].value > cards[j].value ? cards[i].value : cards[j].value;
                        }
                    } else
                        countTwo = 0;
                }
                countTwo = 0;
                for (int j = i + 2; j < 7 - 1; ++j) {
                    if (cards[j].value == cards[j + 1].value) {
                        countTwo++;
                        if (countTwo == 1) {
                            return 10000 + cards[i].value > cards[j].value ? cards[i].value : cards[j].value;
                        }
                    } else
                        countTwo = 0;
                }
            }
        } else {
            countThree = 0;
        }
    }
    return 0;
}

int Game::isFlush(Card* cards) {
    Card* sortedCards = sortCardsBySuit(cards);
    int count = 0;
    for (int i = 0; i < 6; i++) {
        if (cards[i].suit == cards[i + 1].suit) {
            count++;
            if (count == 4) {
                return 5000 + cards[i - 4].value;
            }
        } else {
            count = 0;
        }
    }
    return 0;
}

int Game::isStraight(Card* cards) {
    int count = 0;
    for (int i = 0; i < 7 - 1; i++) {
        if (cards[i].value == cards[i + 1].value + 1) {
            count++;
            if (count == 4) {
                return 1000 + cards[i].value + 4;
            }
        } else {
            count = 0;
        }
    }
    return 0;
}

int Game::isThreeOfAKind(Card* cards) {
    int count = 0;
    for (int i = 0; i < 7 - 1; i++) {
        if (cards[i].value == cards[i + 1].value) {
            count++;
            if (count == 2) {
                return 500 + cards[i].value;
            }
        } else {
            count = 0;
        }
    }
    return 0;
}

int Game::isTwoPair(Card* cards) {
    int countOne = 0;
    int countTwo = 0;
    for (int i = 0; i < 7 - 1; i++) {
        if (cards[i].value == cards[i + 1].value) {
            countOne++;
            if (countOne == 1) {
                for (int j = i + 1; j < 7 - 1; j++) {
                    if (cards[j].value == cards[j + 1].value) {
                        countTwo++;
                        if (countTwo == 1) return 100 + cards[i].value;
                    } else {
                        countTwo = 0;
                    }
                }
            }
        } else {
            countOne = 0;
        };
    }
    return 0;
}

int Game::isPair(Card* cards) {
    int count = 0;
    for (int i = 0; i < 7 - 1; i++) {
        if (cards[i].value == cards[i + 1].value) {
            count++;
            if (count == 1)
                return 50 + cards[i].value;
        } else
            count = 0;
    }
    return 0;
}

Card* Game::sortCardsBySuit(Card* cards) {
    Card* sortedCards = (Card*)malloc(sizeof(Card) * 7);
    for (int i = 0; i < 7; i++) {
        sortedCards[i] = cards[i];
    }
    for (int i = 0; i < 7 - 1; i++) {
        for (int j = i; j < 7; j++) {
            if (sortedCards[i].suit > sortedCards[j].suit) {
                Card tmp = sortedCards[i];
                sortedCards[i] = sortedCards[j];
                sortedCards[j] = sortedCards[i];
            }
        }
    }

    return sortedCards;
}

Card* Game::sortCardsByValue(Card* cards) {
    Card* sortedCards = (Card*)malloc(sizeof(Card) * 7);
    for (int i = 0; i < 7; i++) {
        sortedCards[i] = cards[i];
    }
    for (int i = 0; i < 7 - 1; i++) {
        for (int j = i; j < 7; j++) {
            if (sortedCards[i].value > sortedCards[j].value) {
                Card tmp = sortedCards[i];
                sortedCards[i] = sortedCards[j];
                sortedCards[j] = sortedCards[i];
            }
        }
    }

    return sortedCards;
}

int Game::calculatePoint(int id) {
    if ((*m_playerInfoList)[id].id == -1) {
        return false;
    }
    Card cards[7];
    cards[0] = playerHands[id].cards[0];
    cards[1] = playerHands[id].cards[1];

    for (int i = 2; i < 7; i++) {
        cards[i] = m_communityCards[i - 2];
    }
    Card* sortedCards = sortCardsByValue(cards);
    int point;
    if (point = isStraightFlush(cards) > 0) return point;
    if (point = isFourOfAKind(cards) > 0) return point;
    if (point = isFullHouse(cards) > 0) return point;
    if (point = isFlush(cards) > 0) return point;
    if (point = isStraight(cards) > 0) return point;
    if (point = isThreeOfAKind(cards) > 0) return point;
    if (point = isPair(cards) > 0) return point;
    return sortedCards[0].value;
}

// return winner_id, add prize to the balance
int Game::getResult() {
    if (m_activePlayerCount < 2) {
        for (int i = 0; i < g_maxPlayerCount; ++i) {
            if ((*m_playerInfoList)[i].id != -1 && (*m_playerInfoList)[i].status == PLAYING) {
                (*m_playerInfoList)[i].balance += m_pool;
                return i;
            }
        }
    }
    int result[g_maxPlayerCount];
    for (int i = 0; i < g_maxPlayerCount; ++i) {
        if ((*m_playerInfoList)[i].id != -1 && (*m_playerInfoList)[i].status == PLAYING) {
            result[i] = calculatePoint(i);
        } else
            result[i] = -1;
    }
    int max = -1;
    int maxID = -1;
    for (int i = 0; i < g_maxPlayerCount; ++i) {
        if (max < result[i]) {
            max = result[i];
            maxID = i;
        }
    }
    (*m_playerInfoList)[maxID].balance+= m_pool;
    return maxID;
}
}  // namespace game