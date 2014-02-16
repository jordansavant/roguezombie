#pragma once
#ifndef BIT_CLIENTSERVERSTATE_H
#define BIT_CLIENTSERVERSTATE_H

#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include "State.hpp"
#include "GameTimer.hpp"

namespace bit
{
    class StateStack;
    class Game;
    class Server;

    class ClientServerState : public State
    {
    public:

        ClientServerState(StateStack &stack, Game* game, bool isHost);

        virtual ~ClientServerState();

        virtual void load();

        virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    private:

        bool isHost;
        Server* server;
        sf::IpAddress ipAddress;
        unsigned short port;
        sf::TcpSocket socket;
        bool connected;
        sf::Clock failedConnectionClock;
        sf::Time timeSinceLastPacket;
        sf::Time clientTimeout;
        GameTimer tickTimer;
        sf::Time tickRate;
        sf::Clock tickClock;

        virtual Server* newServer();

        void handlePacket(sf::Int32 packetType, sf::Packet &packet);

        virtual void handlePacket_Broadcast(sf::Packet &packet);

        virtual void handlePacket_InitializeSelf(sf::Packet &packet);

        virtual void handlePacket_InitializeWorld(sf::Packet &packet);

        virtual void handlePacket_PeerConnected(sf::Packet &packet);

        virtual void handlePacket_PeerDisonnected(sf::Packet &packet);

        virtual void handlePacket_ServerUpdate(sf::Packet &packet);

        virtual void handlePacket_PeerEvent(sf::Packet &packet);

        virtual void handlePacket_PeerRealtimeChange(sf::Packet &packet);

        virtual void handlePacket_Shutdown(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ClientUpdate(sf::Packet &packet);

    };
}

#endif