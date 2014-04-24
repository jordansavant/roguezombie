#pragma once
#ifndef BIT_CLIENTSERVERSTATE_H
#define BIT_CLIENTSERVERSTATE_H

#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include "../Game/State.hpp"
#include "../Game/GameTimer.hpp"

namespace bit
{
    class StateStack;
    class Game;
    class Server;
    class ServerPacket;
    class ClientPacket;

    class ClientServerState : public State
    {
    public:

        ClientServerState(StateStack &stack, Game* game, bool isClient, bool isHost);

        virtual ~ClientServerState();

        sf::Uint32 clientId;
        sf::Uint32 lastSnapshotId;

        virtual void load();

        virtual bool update(sf::Time &gameTime);

    protected:

        bool isClient;
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
        bool awaitingDisconnect;
        GameTimer disconnectTimer;

        virtual Server* newServer() = 0;

        virtual std::string getServerIpAddress() = 0;

        virtual unsigned short getServerPort() = 0;

		sf::Time now();

        void handlePacket(sf::Int32 packetType, ServerPacket &packet);

        virtual void disconnect();

        // Packet handling

        virtual void handle_ServerTimeout() = 0;

        virtual void handlePacket_Broadcast(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_InitializeSelf(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_InitializeWorld(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_PeerClientConnected(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_ClientDisonnected(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_ServerUpdate(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_ServerEvent(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_DisconnectAcknowledge(bit::ServerPacket &packet) = 0;

        virtual void handlePacket_Shutdown(bit::ServerPacket &packet) = 0;

        virtual void handle_DisconnectTimeout() = 0;

        virtual void handlePacket_Kick(bit::ServerPacket &packet) = 0;

        // Packet sending

        virtual void preparePacket_ClientInformation(bit::ClientPacket &packet) = 0;

        virtual void preparePacket_ClientUpdate(bit::ClientPacket &packet) = 0;

        virtual void preparePacket_ClientDisconnect(bit::ClientPacket &packet) = 0;
    };
}

#endif