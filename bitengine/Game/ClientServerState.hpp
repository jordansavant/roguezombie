#pragma once
#ifndef BIT_CLIENTSERVERSTATE_H
#define BIT_CLIENTSERVERSTATE_H

#include "SFML/Network.hpp"
#include "State.hpp"

namespace bit
{
    class StateStack;
    class Game;
    class Server;

    class ClientServerState : public State
    {
    public:

        ClientServerState(StateStack &stack, Game* game, bool isHost);

        ~ClientServerState();

    private:

        bool isHost;
        Server* server;

        void handlePacket(sf::Int32 packetType, sf::Packet &packet);
    };
}

#endif