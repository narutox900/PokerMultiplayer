#pragma once

#include "IPEndpoint.hpp"
#include "Player.hpp"
#include "./utils/ReusableIdList.hpp"

#include "../../protobuf/src/network.pb.h"

#include <vector>

class Room {
    private:
    static constexpr uint s_maxPlayerCount = 5;

    ReusableIdList<Player> m_playerList;
    
    int m_ownerID;

    public:
    using iterator = ReusableIdList<Player>::iterator;
    using const_iterator = ReusableIdList<Player>::const_iterator;
    
    Room();
    ~Room() {

    }

    bool is_full() const {
        return m_playerList.size() >= s_maxPlayerCount;
    }
    
    int ownerID() const {
        return m_ownerID;
    }

    int addPlayer(Player player);
    bool removePlayer(uint id);

    const Player* findPlayer(uint id) const;
    const Player* findPlayer(const IPEndpoint& endpoint) const;

    size_t size() const {
        return m_playerList.size();
    }

    const_iterator begin() const {
        return m_playerList.begin();
    }

    const_iterator end() const {
        return m_playerList.end();
    }

    void serializeToRoomInfo(network::RoomInfo& roomInfo) const;
};