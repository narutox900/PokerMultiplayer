#include "Deck.hpp"
#include <algorithm>
#include <random>

namespace game {

Deck::Deck() {
    for (int i = 2; i <= 14; ++i) {
        for (int j = 0; j < 4; ++j) {
            Card card;
            card.suit=j;
            card.value=i;
            m_cards.push_back(card);
        }
    }
}

void Deck::shuffle() {
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(m_cards.begin(), m_cards.end(), rng);
}

Card Deck::deal() {
    Card tmp = m_cards.back();
    m_cards.pop_back();
    return tmp;
}
}  // namespace game
