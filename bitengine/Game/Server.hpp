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
            Broadcast,                  // string broadcast
            InitializeWorld,            // tell client the world to build
            InitializeSelf,             // tell client to build themselves
            PeerClientConnected,        // tell connected clients about a new client
            PeerClientDisconnected,     // tell connected clients about a lost client
            PeerClientEvent,            // tell clients about event from specific client
            PeerClientRealtimeChange,   // tell clients about boolean changes from specific client
            ServerUpdate,               // tick update for world snapshot 1/20th a second
            Shutdown                    // tell connected clients that the server is shutting down
        };

        enum ClientPacket
        {
            Quit,                       // tell server that client is quitting
            Event,                      // tell server about a client event
            RealtimeChange,             // tell server about a client realtime boolean change
            ClientUpdate,               // tell server about client update 1/20th a second
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


        virtual void handlePacket_Event(sf::Packet &packet, RemoteClient &client);

        virtual void handlePacket_RealtimeChange(sf::Packet &packet, RemoteClient &client);


        virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

        virtual sf::Packet& preparePacket_InitializeWorld(sf::Packet &packet);

        virtual sf::Packet& preparePacket_PeerClientConnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_PeerClientDisconnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
    };
}

#endif