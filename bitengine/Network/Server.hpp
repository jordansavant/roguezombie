#pragma once
#ifndef BIT_SERVER_H
#define BIT_SERVER_H

#define BIT_SERVER_PORT 12345
#define BIT_SERVER_TICK_FPS 20.0f
#define BIT_SERVER_UPDATE_FPS 60.0f

#include "SFML/Network.hpp"
#include <functional>

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

        virtual ~Server();

        sf::Mutex isLoadCompleteMutex;
        bool isLoadComplete;

        enum ServerPacketType
        {
            Broadcast,                  // tell all clients a string broadcast
            InitializeSelf,             // tell client we have acknowledged their connection
            InitializeWorld,            // tell client of the full world
            Event,                      // tell client of game event
            DisconnectAcknowledged,     // tell client we have acknowledge their disconnection
            Kick,                       // tell client we have kicked them
            PeerClientConnected,        // tell all clients about a new client
            PeerClientDisconnected,     // tell all clients about a lost client
            ServerUpdate,               // tell client about a world snapshot 1/20th a second
            Shutdown,                   // tell all clients that the server is shutting down
            Response,                   // tell client a response to a particular request
        };

        enum ClientPacketType
        {
            ClientInformation,          // tell server about game specific client connection information
            ClientUpdate,               // tell server about client input update 1/20th a second
            Quit,                       // tell server that client is quitting
            Request,                    // tell server of a request that guarantees a response
        };

        void start();

        void stop();

        // Server Event Packets

        void sendEventToClient(bit::RemoteClient &client, std::function<void(ServerPacket&)> prepare);

        void sendEventToAllClients(std::function<void(ServerPacket&)> prepare);

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

        virtual unsigned short getServerPort();

        void handleIncomingPackets();

        void handleConnections();

        void handlePacket(ClientPacket &packet, RemoteClient &receivingClient);

        void handleDisconnections();

        void handleNewClient(RemoteClient &client);

        void sendWorldInitialization(RemoteClient &client);

        void sendToAllClients(ServerPacket &packet);

        void kickClient(bit::RemoteClient &client, unsigned int kickCode);


        // Packet handling

        virtual void handlePacket_ClientInformation(ClientPacket &packet, RemoteClient &client) = 0;

        virtual void handlePacket_ClientUpdate(ClientPacket &packet, RemoteClient &client) = 0;

        virtual void handlePacket_ClientDisconnect(ClientPacket &packet, RemoteClient &client) = 0;

        virtual void handle_ClientTimeout(RemoteClient &client) = 0;

        virtual void handlePacket_ClientRequest(ClientPacket &packet, RemoteClient &client, ServerPacket &responsePacket) = 0;


        // Packet sending

        virtual void preparePacket_InitializeSelf(ServerPacket &packet, RemoteClient &client) = 0;

        virtual void preparePacket_InitializeWorld(ServerPacket &packet, RemoteClient &client) = 0;

        virtual void preparePacket_PeerClientConnected(ServerPacket &packet) = 0;

        virtual void preparePacket_PeerClientDisconnected(ServerPacket &packet) = 0;

        virtual void preparePacket_ServerUpdate(ServerPacket &packet, RemoteClient &client) = 0;

        virtual void preparePacket_DisconnectAcknowledge(ServerPacket &packet, RemoteClient &client) = 0;

        virtual void preparePacket_Shutdown(ServerPacket &packet) = 0;
    };
}

#endif