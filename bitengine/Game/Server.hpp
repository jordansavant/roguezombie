#pragma once
#ifndef BIT_SERVER_H
#define BIT_SERVER_H

#define BIT_SERVER_PORT 12345
#define BIT_SERVER_TICK_FPS 20.0f
#define BIT_SERVER_UPDATE_FPS 60.0f

#include "SFML/Network.hpp"

namespace bit
{
    class Game;
    class RemoteClient;
    class ServerPacket;
    class ClientPacket;

    class Server
    {
    public:

        Server();

        ~Server();

        enum ServerPacketType
        {
            Broadcast,                  // string broadcast
            InitializeSelf,             // tell client we have acknowledged their connection
            PeerClientConnected,        // tell connected clients about a new client
            PeerClientDisconnected,     // tell connected clients about a lost client
            ServerUpdate,               // tick update for world snapshot 1/20th a second
            Shutdown                    // tell connected clients that the server is shutting down
        };

        enum ClientPacketType
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

        void handlePacket(ClientPacket &packet, RemoteClient &receivingClient, bool &detectedTimeout);

        void handleConnections();

        void handleDisconnections();

        void broadcastMessage(std::string &message);

        void sendToAllClients(ServerPacket &packet);

        void handleNewClient(RemoteClient &client);

        virtual unsigned short getServerPort();

        // Packet handling

        virtual void handlePacket_ClientInformation(ClientPacket &packet, RemoteClient &client) = 0;

        virtual void handlePacket_ClientUpdate(ClientPacket &packet, RemoteClient &client) = 0;

        // Packet sending

        virtual void preparePacket_InitializeSelf(ServerPacket &packet) = 0;

        virtual void preparePacket_PeerClientConnected(ServerPacket &packet) = 0;

        virtual void preparePacket_PeerClientDisconnected(ServerPacket &packet) = 0;

        virtual void preparePacket_ServerUpdate(ServerPacket &packet) = 0;
    };
}

#endif