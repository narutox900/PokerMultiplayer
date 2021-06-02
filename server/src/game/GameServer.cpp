#include "GameServer.hpp"

namespace game {
#define DEFAULTBALANCE 200;

GameServer::GameServer(size_t roomID, uint16_t port)
    : m_roomId(roomID), m_socket(port, g_maxPlayerCount), m_playerInfoList{}, m_buffer(new uint8_t[s_bufferSize]) {
    for (auto& player : m_playerInfoList) {
        player.id = -1;
    }
    m_ownerID = -1;
}

bool GameServer::addPlayer(const IPEndpoint& endpoint, const int16_t sockfd) {
    int id = 0;
    for (id = 0; id < g_maxPlayerCount; id++) {
        if (m_playerInfoList[id].id == -1) {
            break;
        }
    }
    if (id >= g_maxPlayerCount) {
        return false;
    }
    m_playerInfoList[id].id = id;
    m_playerInfoList[id].endpoint = endpoint;
    m_playerInfoList[id].sockfd = sockfd;
    m_playerInfoList[id].connectionState = PlayerInfo::ConnectionState::Connected;
    m_playerInfoList[id].balance = DEFAULTBALANCE;
    m_playerInfoList[id].currentBet = 0;
    if (m_ownerID == -1) {
        m_ownerID = id;
    }
    printf("New player added %d, %d!\n", id, sockfd);

    return true;
}

bool GameServer::removePlayer(int id) {
    if (m_playerInfoList[id].id == -1) {
        return false;
    }
    m_playerInfoList[id].id = -1;
    if (m_ownerID == id) {
        for (int i = 0; i < 5; i++) {
            if (m_playerInfoList[i].id != -1) {
                m_ownerID = i;
                break;
            }
        }
        if (m_ownerID == id) m_ownerID = -1;
    }
    return true;
}

void GameServer::start() {
    m_thread = std::thread(&GameServer::startGameServer, this);  // TODO handler
    // startGameServer();
}

void GameServer::wait() {
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void GameServer::startGameServer() {
    int nready, i, maxi, port, connfd, sockfd;
    struct sockaddr_in cliaddr, servaddr;

    ssize_t n;
    int INFTIM = -1;
    maxi = g_maxPlayerCount;  // max index into the clients[] array

    // The server socket
    clients[0].fd = m_socket.socketFd();
    clients[0].events = POLLRDNORM;

    printf("GameServer %ld start polling\n", m_roomId);

    for (i = 1; i < 10; i++) {
        clients[i].fd = -1;  //avaiable entries
    }

    while (1) {
        nready = poll(clients, maxi + 1, INFTIM);

        printf("While loop\n");
        if (nready <= 0) {
            continue;
        }

        // Check for new connection
        if (clients[0].revents & POLLRDNORM) {
            connfd = m_socket.accept(&cliaddr);
            if (connfd < 0) {
                printf("ERROR connfd <0 at gameserver\n");
                throw;
            }

            printf("Accept game server socket %d (%s : %hu)\n",
                   connfd,
                   inet_ntoa(cliaddr.sin_addr),
                   ntohs(cliaddr.sin_port));

            addPlayer(IPEndpoint(cliaddr), connfd);

            // Save client socket into clients array
            for (i = 0; i < maxi + 1; i++) {
                if (clients[i].fd < 0) {
                    clients[i].fd = connfd;

                    clients_addr[i] = cliaddr;
                    break;
                }
            }

            // No enough space in clients array
            if (i == maxi + i) {
                fprintf(stderr, "Error: too many clients\n");
                close(connfd);
            }

            clients[i].events = POLLRDNORM;

            if (i > maxi) {
                maxi = i;
            }

            // No more readable file descriptors
            if (--nready <= 0) {
                continue;
            }
        }

        for (i = 1; i <= maxi; i++) {
            if ((sockfd = clients[i].fd) < 0) {
                continue;
            }
            // If the client is readable or errors occur
            if (clients[i].revents & (POLLRDNORM | POLLERR)) {
                //TODO handler
                // Get message length
                n = recv(sockfd, m_buffer.get(), sizeof(int), 0);
                if (n == -1) {
                    throw;
                }
                if (n == 0) {
                    close(clients[i].fd);
                    clients[i].fd = -1;
                    continue;
                }
                IPEndpoint client{clients_addr[i]};
                printf("GameRoom Received %ld byte packet: %s:%d\n", n, client.addressAsStr(), client.port());
                int* length = (int*)m_buffer.get();
                printf("Length+1 of payload is %d\n", *length);

                n = recv(sockfd, m_buffer.get(), *length + 1, 0);
                printf("Gameroom receiving %ld bytes\n", n);
                if (n == -1) {
                    printf("Socket read error: errorno %d", errno);
                    break;
                }

                if (n >= s_bufferSize) {
                    printf("n >= s_buffersize\n");
                    continue;
                }
                handleMessage(sockfd, m_buffer.get(), n, client);
            }
        }
    }
}

void GameServer::handleMessage(int sockfd, const uint8_t* buffer, size_t size, const IPEndpoint& client) {
    game::MessageType mesgType = (game::MessageType)buffer[0];
    if (mesgType == game::MessageType::StartGameRequest) {
        if (sockfd != m_playerInfoList[m_ownerID].sockfd) return;
        game::StartGameResponse response{};

        response.set_dealer_id(m_ownerID);

        for (int i = 0; i < g_maxPlayerCount; ++i) {
            if (m_playerInfoList[i].id == -1) continue;
            if (m_playerInfoList[i].balance < 2 * g_baseBetValue) {
                m_playerInfoList[i].status = NOT_PLAYING;
                continue;
            }
            m_playerInfoList[i].status = PLAYING;
            m_playerInfoList[i].currentBet = 0;
            game::ProtoPlayer* player = response.add_players();
            player->set_id(m_playerInfoList[i].id);
            player->set_balance(m_playerInfoList[i].balance);
            player->set_status(m_playerInfoList[i].status);
            gameInstance.m_activePlayerCount++;
        }
        if (gameInstance.m_activePlayerCount < 2) {
            response.set_success(false);
        }
        response.set_success(true);
        for (int i = 0; i < g_maxPlayerCount; ++i) {
            if (m_playerInfoList[i].id == -1) continue;
            sendMessage(m_playerInfoList[i].endpoint, game::MessageType::StartGameResponse, response, m_playerInfoList[i].sockfd);
            printf("Sending startgame message to player id %d\n", i);
        }
        if (response.success() == false) return;
        startGameInstance();
    }
}

void GameServer::startGameInstance() {
    // start new game instance
    gameInstance = Game(m_playerInfoList);
    int firstID = m_ownerID;
    // All people bet the base value
    for (int i = 0; i < g_maxPlayerCount; ++i) {
        if (m_playerInfoList[i].id != -1) {
            gameInstance.dealPlayerCards(i);
            // send  cards info to player
            Card card0 = gameInstance.playerHands[i].cards[0];
            Card card1 = gameInstance.playerHands[i].cards[1];
            game::DealCards dealCard{};
            game::ProtoCard* protocard0 = dealCard.add_cards();
            protocard0->set_suit(card0.suit);
            protocard0->set_value(card0.value);
            game::ProtoCard* protocard1 = dealCard.add_cards();
            protocard1->set_suit(card1.suit);
            protocard1->set_value(card1.value);
            printf("Sending deal card message to player %d\n",i);
            sendMessage(m_playerInfoList[i].endpoint, game::MessageType::DealCards, dealCard, m_playerInfoList[i].sockfd);
            // default bet amount
            m_playerInfoList[i].balance -= g_baseBetValue;
            m_playerInfoList[i].currentBet = g_baseBetValue;
            gameInstance.m_pool += g_baseBetValue;
        }
    }
    gameInstance.m_currentBet = g_baseBetValue;
    // broadcast betturn message
    int currentID = firstID;
    broadcastBetTurnMessage(currentID, gameInstance.m_pool, gameInstance.m_currentBet - m_playerInfoList[currentID].currentBet, m_playerInfoList[currentID].balance);
    //turn 1
    recvBetResponseMessage(currentID);
    int phase = 0;
    gameInstance.m_endTurnID = currentID;
    while (1) {
        currentID++;
        // circular count
        if (currentID >= g_maxPlayerCount) {
            currentID = 0;
        }
        //reach end turn
        if (currentID == gameInstance.m_endTurnID) break;
        // if doesn't exist player continue
        if (m_playerInfoList[currentID].id == -1) continue;
        if (m_playerInfoList[currentID].status == NOT_PLAYING) continue;
        broadcastBetTurnMessage(currentID, gameInstance.m_pool, gameInstance.m_currentBet - m_playerInfoList[currentID].currentBet, m_playerInfoList[currentID].balance);
        recvBetResponseMessage(currentID);
    }
    sendEndRoundMessage(gameInstance.m_pool);
    phase++;
    // Deal community card
    dealCommunityCard(phase);

    // phase 2 3 4 is the same
    while (1) {
        if (phase == 5) break;
        gameInstance.m_currentBet = 0;
        // one time for the first player
        gameInstance.m_endTurnID = firstID;
        if (m_playerInfoList[firstID].status == PLAYING && m_playerInfoList[firstID].id != -1) {
            broadcastBetTurnMessage(firstID, gameInstance.m_pool, gameInstance.m_currentBet - m_playerInfoList[firstID].currentBet, m_playerInfoList[firstID].balance);
            recvBetResponseMessage(firstID);
        }
        // while loop for the rest
        currentID = firstID;
        while (1) {
            currentID++;
            // circular count
            if (currentID >= g_maxPlayerCount) {
                currentID = 0;
            }
            //reach end turn
            if (currentID == gameInstance.m_endTurnID) break;
            // if doesn't exist player continue
            if (m_playerInfoList[currentID].id == -1) continue;
            if (m_playerInfoList[currentID].status == NOT_PLAYING) continue;
            broadcastBetTurnMessage(currentID, gameInstance.m_pool, gameInstance.m_currentBet - m_playerInfoList[currentID].currentBet, m_playerInfoList[currentID].balance);
            recvBetResponseMessage(currentID);
        }
        sendEndRoundMessage(gameInstance.m_pool);
        phase++;
        // Deal community card
        dealCommunityCard(phase);
    }
    broadcastResultMessage();
}

void GameServer::broadcastResultMessage() {
    int winner_id = gameInstance.getResult();
    game::Result resultMessage{};
    resultMessage.set_prize(gameInstance.m_pool);
    resultMessage.set_winner_id(winner_id);
    for (int i = 0; i < g_maxPlayerCount; ++i) {
        if (m_playerInfoList[i].id == -1) {
            continue;
        }
        game::ProtoPlayer* player;
        player->set_id(i);
        player->set_balance(m_playerInfoList[i].balance);
        player->set_status(m_playerInfoList[i].status);
        game::Result_EndPlayer* endPlayer = resultMessage.add_end_players();
        endPlayer->set_allocated_player(player);
        if (m_playerInfoList[i].status == PLAYING) {
            game::ProtoCard* card0 = endPlayer->add_cards();
            card0->set_value(gameInstance.playerHands[i].cards[0].value);
            card0->set_suit(gameInstance.playerHands[i].cards[0].suit);
            game::ProtoCard* card1 = endPlayer->add_cards();
            card1->set_value(gameInstance.playerHands[i].cards[1].value);
            card1->set_suit(gameInstance.playerHands[i].cards[1].suit);
        }
    }
    for (int i = 0; i < g_maxPlayerCount; ++i) {
        if (m_playerInfoList[i].id == -1) continue;
        sendMessage(m_playerInfoList[i].endpoint, game::MessageType::Result, resultMessage, m_playerInfoList[i].sockfd);
        printf("Sending result message to player %d\n",i);
    }
}

void GameServer::dealCommunityCard(int phase) {
    // if first phase deal 2 else deal 1
    game::DealCommunityCards dealCommunityMessage{};
    dealCommunityMessage.set_phase(phase);
    if (phase == 1) {
        Card tmp = gameInstance.dealCommunityCard();
        game::ProtoCard* first_card = dealCommunityMessage.add_cards();
        first_card->set_suit(tmp.suit);
        first_card->set_value(tmp.value);
        tmp = gameInstance.dealCommunityCard();
        game::ProtoCard* second_card = dealCommunityMessage.add_cards();
        second_card->set_suit(tmp.suit);
        second_card->set_value(tmp.value);
    } else if (phase == 5) {
        return;
    } else {
        Card tmp = gameInstance.dealCommunityCard();
        game::ProtoCard* card = dealCommunityMessage.add_cards();
        card->set_value(tmp.value);
        card->set_suit(tmp.suit);
    }
    for (int i = 0; i < g_maxPlayerCount; i++) {
        if (m_playerInfoList[i].id == -1) continue;
        sendMessage(m_playerInfoList[i].endpoint, game::MessageType::DealCommunityCards, dealCommunityMessage, m_playerInfoList[i].sockfd);
        printf("Sending deal community card message to player %d\n",i);
    }
}

void GameServer::sendEndRoundMessage(int total_amount) {
    game::EndRound endRoundMessage{};
    endRoundMessage.set_total_amount(total_amount);
    for (int i = 0; i < g_maxPlayerCount; i++) {
        if (m_playerInfoList[i].id == -1) continue;
        sendMessage(m_playerInfoList[i].endpoint, game::MessageType::EndRound, endRoundMessage, m_playerInfoList[i].sockfd);
        // reset current bet
        printf("Sending end round message to player %d\n",i);
        m_playerInfoList[i].currentBet = 0;
    }
    // reset current bet
    gameInstance.m_currentBet = 0;
}

void GameServer::recvBetResponseMessage(int id) {
    int sockfd = m_playerInfoList[id].sockfd;
    int n = recv(sockfd, m_buffer.get(), sizeof(int), 0);
    printf("Receive %d byte int \n", n);
    if (n == -1) {
        throw;
    }
    //in case disconnect message
    if (n == 0) {
        for (int i = 1; i <= g_maxPlayerCount; i++) {
            if (clients[i].fd == sockfd) {
                close(sockfd);
                clients[i].fd = -1;
                m_playerInfoList[id].status = NOT_PLAYING;
                m_playerInfoList[id].id = -1;
                uint8_t* buffer = m_buffer.get();
                game::BetTurnResponse response{};
                gameInstance.foldPlayer(id);
            }
            sendDoneBetMessage(id, 0, m_playerInfoList[id].balance, action::FOLD);
            return;
        }
    }

    int* length = (int*)m_buffer.get();

    n = recv(sockfd, m_buffer.get(), *length + 1, 0);
    printf("receiving %d bytes\n", n);
    if (n == -1) {
        printf("Socket read error: errorno %d", errno);
    }
    uint8_t* buffer = m_buffer.get();
    game::MessageType mesgType = (game::MessageType)buffer[0];
    if (mesgType == game::MessageType::BetTurnResponse) {
        game::BetTurnResponse response{};
        bool parseResult = response.ParseFromArray(buffer, n - 1);
        assert(parseResult);
        int amount = response.amount();
        switch (response.action()) {
            case game::action::CALL:
                gameInstance.callPlayer(id);
                break;
            case game::action::BET:
                gameInstance.raisePlayer(id, amount);
                break;
            case game::action::FOLD:
                gameInstance.foldPlayer(id);
                break;
            default:
                break;
        }
        sendDoneBetMessage(id, amount, m_playerInfoList[id].balance, response.action());
    } else {
        // unwanted behavior
        throw;
    }
}

void GameServer::sendDoneBetMessage(int id, int amount, int balance, int action) {
    game::DoneBet doneBetMessage{};
    doneBetMessage.set_player_id(id);
    doneBetMessage.set_action(action);
    doneBetMessage.set_bet_amount(amount);
    doneBetMessage.set_player_balance(balance);

    for (int i = 0; i < g_maxPlayerCount; i++) {
        if (m_playerInfoList[i].id == -1) continue;
        printf("Sending done bet message to player %d\n",i);
        sendMessage(m_playerInfoList[i].endpoint, game::MessageType::DoneBet, doneBetMessage, m_playerInfoList[i].sockfd);
    }
}

void GameServer::broadcastBetTurnMessage(int id, int currentPool, int amount, int balance) {
    game::BetTurn betTurnMessage{};
    betTurnMessage.set_player_id(id);
    betTurnMessage.set_current_pool(currentPool);
    betTurnMessage.set_amount(amount);
    betTurnMessage.set_balance(balance);
    for (int i = 0; i < g_maxPlayerCount; i++) {
        if (m_playerInfoList[i].id == -1) continue;
        printf("Sending bet turn message to player %d\n",i);
        sendMessage(m_playerInfoList[i].endpoint, game::MessageType::BetTurn, betTurnMessage, m_playerInfoList[i].sockfd);
    }
}

void GameServer::sendMessage(const IPEndpoint& receiver, game::MessageType type, const google::protobuf::Message& message, int sockfd) {
    uint8_t* buffer = m_buffer.get();

    //write message
    size_t messageSize = message.ByteSizeLong();
    int* length = (int*)(&buffer[0]);
    *length = messageSize;
    // write type into buffer
    buffer[sizeof(int)] = (uint8_t)type;
    bool ret = message.SerializeToArray(buffer + 1 + sizeof(int), messageSize);
    assert(ret);

    send(sockfd, buffer, messageSize + 1 + sizeof(int), 0);
}

}  // namespace game
