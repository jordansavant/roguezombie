#pragma once
#ifndef BIT_CLIENTSERVERSTATE_H
#define BIT_CLIENTSERVERSTATE_H

#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include "../Game/State.hpp"
#include "../Game/GameTimer.hpp"
#include <functional>
#include <map>
#include <deque>
#include "ServerPacket.hpp"
#include "ClientPacket.hpp"

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

        ClientServerState(StateStack &stack, Game* game, bool isClient, bool isHost, bool isLocalOnly);

        virtual ~ClientServerState();

        sf::Uint32 clientId;
        sf::Uint32 lastSnapshotId;

        virtual void load();

        virtual bool update(sf::Time &gameTime);

        void serverRequest(std::function<void(ClientPacket&)> prepare, std::function<void(ServerPacket&)> onComplete);

        void direct_serverSendToClient(ServerPacket &packet);

        bool direct_receiveFromServer(ServerPacket &packet);

        void direct_clientSendToServer(ClientPacket &packet);

        bool direct_receiveFromClient(ClientPacket &packet);

    protected:

        struct Request
        {
            Request()
                : id(0), isAbandonded(false), onComplete(NULL)
            {
            }

            unsigned int id;
            bool isAbandonded;
            std::function<void(bit::ServerPacket &packet)> onComplete;
        };

        bool isClient;
        bool isHost;
        bool isLocalOnly;
        Server* server;
        sf::IpAddress ipAddress;
        unsigned short port;
        sf::TcpSocket socket;
        bool isNetworkConnected; // Network socket connected
        bool isConfirmed; // Active and confirmed game relationship
        sf::Clock failedConnectionClock;
        sf::Time timeSinceLastPacket;
        sf::Time clientTimeout;
        GameTimer tickTimer;
        sf::Time tickRate;
        sf::Clock tickClock;
        sf::Clock clock;
        bool awaitingDisconnect;
        GameTimer disconnectTimer;
        std::map<unsigned int, Request> requests;
        unsigned int requestCounter;
        std::deque<ServerPacket> direct_serverToClientPackets;
        std::deque<ClientPacket> direct_clientToServerPackets;
        sf::Mutex directConnectMutex;

        virtual Server* newServer() = 0;

        virtual std::string getServerIpAddress() = 0;

        virtual unsigned short getServerPort() = 0;

        sf::Time now();

        void handlePacket(sf::Uint32 packetType, ServerPacket &packet);

        virtual void disconnect();

        bool hasServerLoadCompleted();


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