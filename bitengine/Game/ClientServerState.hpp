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

    protected:

        sf::Uint32 lastSnapshotId;
        bool isHost;
        Server* server;
        sf::IpAddress ipAddress;
        unsigned short port;
        sf::TcpSocket socket;
        bool isConnected;
        bool isConfirmed;
        sf::Clock failedConnectionClock;
        sf::Time timeSinceLastPacket;
        sf::Time clientTimeout;
        GameTimer tickTimer;
        sf::Time tickRate;
        sf::Clock tickClock;
	    sf::Clock clock;

        virtual Server* newServer();

		sf::Time now();

        void handlePacket(sf::Int32 packetType, sf::Packet &packet);

        // Packet handling

        virtual void handlePacket_Broadcast(sf::Packet &packet) = 0;

        virtual void handlePacket_InitializeSelf(sf::Packet &packet) = 0;

        virtual void handlePacket_PeerClientConnected(sf::Packet &packet) = 0;

        virtual void handlePacket_ClientDisonnected(sf::Packet &packet) = 0;

        virtual void handlePacket_ServerUpdate(sf::Packet &packet) = 0;

        virtual void handlePacket_Shutdown(sf::Packet &packet) = 0;

        // Packet sending

        virtual void preparePacket_ClientInformation(sf::Packet &packet) = 0;

        virtual void preparePacket_ClientUpdate(sf::Packet &packet) = 0;

    };
}

#endif