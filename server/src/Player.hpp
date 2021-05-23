#pragma once

#include "IPEndpoint.hpp"

#include <string>

class Player {
    private:
    std::string m_name;
    IPEndpoint m_endpoint;
    int m_sockfd;

    public:
    Player() = default;

    Player(std::string name, IPEndpoint endpoint, int sockfd) {
        m_name = name;
        m_endpoint = endpoint;
        m_sockfd = sockfd;
    }

    const std::string& name() const {
        return m_name;
    }

    const IPEndpoint& endpoint() const {
        return m_endpoint;
    }

    const int sockfd() const {
        return m_sockfd;
    }
};