#pragma once
#ifndef BIT_SERVER_H
#define BIT_SERVER_H

#include "SFML/Network.hpp"

namespace bit
{
    class Game;

    class Server
    {
    public:

        Server();

        ~Server();

        enum ServerPacket
        {
            Broadcast,
            InitializeSelf,
            InitializeWorld,
            PeerConnected,
            PeerDisconnected,
            ServerUpdate,
            PeerEvent,
            PeerRealtimeChange,
            Shutdown
        };

        enum ClientPacket
        {
            Quit,
            PlayerEvent,
            PlayerRealtimeChange,
            ClientUpdate,
            GameEvent
        };

        struct RemotePeer
        {
            RemotePeer();

            sf::TcpSocket socket;
            sf::Time lastPacketTime;
            bool isReady;
            bool hasTimedOut;
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
        std::vector<RemotePeer*> peers;

        void setListeningState(bool state);

        void executionThread();

        virtual void update();

        virtual void tick();

        sf::Time now();

        void handleIncomingPackets();

        void handlePacket(sf::Packet &packet, RemotePeer &receivingPeer, bool &detectedTimeout);

        void handleConnections();

        void handleDisconnections();

        void broadcastMessage(std::string &message);

        void sendToAllPeers(sf::Packet &packet);

        virtual void handlePacket_PlayerEvent(sf::Packet &packet, RemotePeer &peer);

        virtual void handlePacket_PlayerRealtimeChange(sf::Packet &packet, RemotePeer &peer);

        virtual sf::Packet& preparePacket_InitializeSelf(sf::Packet &packet);

        virtual sf::Packet& preparePacket_InitializeWorld(sf::Packet &packet);

        virtual sf::Packet& preparePacket_PeerConnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_PeerDisconnected(sf::Packet &packet);

        virtual sf::Packet& preparePacket_ServerUpdate(sf::Packet &packet);
    };
}

#endif