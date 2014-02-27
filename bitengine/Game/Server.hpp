#pragma once
#ifndef BIT_SERVER_H
#define BIT_SERVER_H

#define BIT_SERVER_PORT 12345
#define BIT_SERVER_TICK_FPS 5.0f
#define BIT_SERVER_UPDATE_FPS 60.0f

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
            InitializeSelf,             // tell client we have acknowledged their connection
            PeerClientConnected,        // tell connected clients about a new client
            PeerClientDisconnected,     // tell connected clients about a lost client
            ServerUpdate,               // tick update for world snapshot 1/20th a second
            Shutdown                    // tell connected clients that the server is shutting down
        };

        enum ClientPacket
        {
            ClientInformation,          // tell server about game specific client information
            ClientUpdate,               // tell server about client update 1/20th a second
            Quit,                       // tell server that client is quitting
        };

		void start();

    protected:

        sf::Uint32 snapshotId;
        unsigned short serverPort;
        sf::Thread thread;
        bool waitingThreadEnd;
        sf::Clock clock;
        sf::TcpListener listenerSocket;
        bool isListening;
        sf::Time clientTimeoutTime;
        unsigned int maxConnectedClients;
        unsigned int connectedClients;
		unsigned int clientIdentifier;
        std::vector<RemoteClient*> clients;

        void setListeningState(bool state);

        void executionThread();

        virtual void load();

        virtual void update(sf::Time &gameTime);

        virtual void tick();

        sf::Time now();

        void handleIncomingPackets();

        void handlePacket(sf::Packet &packet, RemoteClient &receivingClient, bool &detectedTimeout);

        void handleConnections();

        void handleDisconnections();

        void broadcastMessage(std::string &message);

        void sendToAllClients(sf::Packet &packet);

        void handleNewClient(RemoteClient &client);

        // Packet handling

        virtual void handlePacket_ClientInformation(sf::Packet &packet, RemoteClient &client);

        virtual void handlePacket_ClientUpdate(sf::Packet &packet, RemoteClient &client);

        // Packet sending

        virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

        virtual sf::Packet& preparePacket_PeerClientConnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_PeerClientDisconnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
    };
}

#endif