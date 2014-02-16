#pragma once
#ifndef BIT_SERVER_H
#define BIT_SERVER_H

#include "SFML/Network.hpp"

namespace bit
{
    class Game;
    class RemoteClient;

    class Server
    {
    public:

        Server();

        ~Server();

        enum ServerPacket
        {
            Broadcast,              // string broadcast
            InitializeWorld,        // tell client the world to build
            InitializeSelf,         // tell client to build themselves
            ClientConnected,        // tell connected clients about a new client
            ClientDisconnected,     // tell connected clients about a lost client
            ServerUpdate,           // tick update for world snapshot 1/20th a second
            ClientEvent,            // tell clients about event from specific client
            ClientRealtimeChange,   // tell clients about boolean changes from specific client
            Shutdown                // tell connected clients that the server is shutting down
        };

        enum ClientPacket
        {
            Quit,                   // tell server that client is quitting
            PlayerEvent,            // tell server about a client event
            PlayerRealtimeChange,   // tell server about a client realtime boolean change
            ClientUpdate,           // tell server about client update 1/20th a second
            GameEvent               // tell server about game event from client ???
        };

    private:

        unsigned short serverPort;
        sf::Thread thread;
        bool waitingThreadEnd;
        sf::Clock clock;
        sf::TcpListener listenerSocket;
        bool isListening;
        sf::Time clientTimeoutTime;
        unsigned int maxConnectedClients;
        unsigned int connectedClients;
        std::vector<RemoteClient*> clients;

        void setListeningState(bool state);

        void executionThread();

        virtual void update();

        virtual void tick();

        sf::Time now();

        void handleIncomingPackets();

        void handlePacket(sf::Packet &packet, RemoteClient &receivingClient, bool &detectedTimeout);

        void handleConnections();

        void handleDisconnections();

        void broadcastMessage(std::string &message);

        void sendToAllClients(sf::Packet &packet);

        virtual void handlePacket_PlayerEvent(sf::Packet &packet, RemoteClient &client);

        virtual void handlePacket_PlayerRealtimeChange(sf::Packet &packet, RemoteClient &client);

        virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

        virtual sf::Packet& preparePacket_InitializeWorld(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ClientConnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ClientDisconnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
    };
}

#endif