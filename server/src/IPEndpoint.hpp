#pragma once

#include <arpa/inet.h>

class IPEndpoint {
   private:
    sockaddr_in m_sa;

   public:
    IPEndpoint() = default;
    ~IPEndpoint() {
    };

    IPEndpoint(uint32_t address, uint16_t port) {
        m_sa = {};
        m_sa.sin_family = AF_INET;
        m_sa.sin_port = htons(port);
        m_sa.sin_addr.s_addr = htonl(address);
    }

    IPEndpoint(const sockaddr_in& sa) {
        m_sa = sa;
    }

    constexpr size_t size() const {
        return sizeof(m_sa);
    }

    sockaddr* get() {
        return (sockaddr*)&m_sa;
    }

    const sockaddr* getConst() const {
        return (const sockaddr*)&m_sa;
    }

    uint16_t port() const {
        return ntohs(m_sa.sin_port);
    }

    uint32_t address() const {
        return ntohl(m_sa.sin_addr.s_addr);
    }

    const char* addressAsStr() const {

        return inet_ntoa(m_sa.sin_addr);
    }

    friend bool operator==(const IPEndpoint& left, const IPEndpoint& right) {
        return left.m_sa.sin_addr.s_addr == right.m_sa.sin_addr.s_addr && left.m_sa.sin_port == right.m_sa.sin_port;
    }
    friend bool operator!=(const IPEndpoint& left, const IPEndpoint& right) {
        return !(left == right);
    }
};
