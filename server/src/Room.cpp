#include "Room.hpp"

Room::Room() : m_playerList(s_maxPlayerCount), m_ownerID(-1) {}

int Room::addPlayer(Player player) {
    const auto [iterator, result] = m_playerList.add(std::move(player));

    if (!result) {
        return -1;
    }
    int newId = iterator->first;

    // Assign the first player entering the room as owner
    if (m_playerList.size() == 1) {
        m_ownerID = newId;
    }

    return newId;
}

bool Room::removePlayer(uint id) {
    bool removeResult = m_playerList.remove(id);
    if (!removeResult) {
        return false;
    }

    if (id == m_ownerID && m_playerList.size() > 0) {
        auto iter = m_playerList.begin();
        m_ownerID = iter->first;
    }
    
    return true;
}

const Player* Room::findPlayer(uint id) const {
    return m_playerList.get(id);
}

const Player* Room::findPlayer(const IPEndpoint& endpoint) const {
    for (auto& [id,player] : m_playerList) {
        if (player.endpoint() == endpoint) {
            return &player;
        }
    }
    return nullptr;
}

void Room::serializeToRoomInfo(network::RoomInfo& roomInfo) const {
    roomInfo.set_is_full(is_full());
    roomInfo.set_owner_id(m_ownerID);

    for (const auto& [id,player] : m_playerList) {
        network::ClientInfo* clientInfo = roomInfo.add_clients();
        clientInfo->set_name(player.name());
        clientInfo->set_id(id);
    }
}