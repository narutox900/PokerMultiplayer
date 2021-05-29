#pragma once
#include <vector>

#include "Card.hpp"

namespace game {
class Deck {
   private:
    std::vector<Card> m_cards;

   public:
    Deck();
    Card deal();
    void shuffle();
};
}  // namespace game